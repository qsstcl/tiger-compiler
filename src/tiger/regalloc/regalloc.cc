#include "tiger/regalloc/regalloc.h"

#include "tiger/output/logger.h"
#include <iostream>
#include <map>
extern frame::RegManager *reg_manager;
extern std::map<std::string, std::pair<int, int>> frame_info_map;
namespace ra {
/* TODO: Put your lab6 code here */


RegAllocator::RegAllocator(std::string frame_name, std::unique_ptr<cg::AssemInstr> assem_instr)
    :frame_name_(frame_name),
    assem_instr_(std::move(assem_instr)),
    color_(nullptr)
{
    color_ = new col::Color(assem_instr_->GetInstrList());
}
RegAllocator::~RegAllocator(){

}

void RegAllocator::RegAlloc(){
    while(true){
        auto color_res = color_->RAColor();
        if(color_res.spills != nullptr){
            RewriteProgram(color_res.spills);
        }
        else{
            coloring_ = color_res.coloring;
            break;
        }
    }
}

std::unique_ptr<ra::Result> RegAllocator::TransferResult(){
    auto res = std::make_unique<ra::Result>();
    res->coloring_ = coloring_;
    
    auto delete_move_list = new assem::InstrList();
    for(auto instr_it = assem_instr_->GetInstrList()->GetList().begin();instr_it != assem_instr_->GetInstrList()->GetList().end();++instr_it){
        if(typeid(*(*instr_it)) == typeid(assem::MoveInstr)){
            auto move_instr = static_cast<assem::MoveInstr*>(*instr_it);
            auto src_reg = coloring_->Look(move_instr->src_->NthTemp(0));
            auto dst_reg = coloring_->Look(move_instr->dst_->NthTemp(0));
            if(src_reg == dst_reg){
                delete_move_list->Append(*instr_it);
            }
        }
    }
    auto coalesced_list = delete_move_list->GetList();
    for(auto instr : coalesced_list){
        assem_instr_->GetInstrList()->Remove(instr);
    }

    res->il_ = assem_instr_->GetInstrList();
    return std::move(res);
}
void RegAllocator::RewriteProgram(live::INodeListPtr spilledNodes) {
    auto spilled_list = spilledNodes->GetList();
    auto& frame_info = frame_info_map[frame_name_];
    auto& frame_offset = frame_info.first;
    auto& frame_size = frame_info.second;

    for (auto v : spilled_list) {
        auto v_temp = v->NodeInfo();
        frame_offset -= 8;
        frame_size += 8;
        auto v_access = frame_offset;

        RewriteInstructions(v_temp, v_access);
    }

    frame_info = std::make_pair(frame_offset, frame_size);
}

void RegAllocator::RewriteInstructions(temp::Temp* v_temp, int v_access) {
    for (auto instr_it = assem_instr_->GetInstrList()->GetList().begin();
         instr_it != assem_instr_->GetInstrList()->GetList().end();
         ++instr_it) {
        if (auto oper_instr = dynamic_cast<assem::OperInstr*>(*instr_it)) {
            RewriteOperInstr(oper_instr, v_temp, v_access, instr_it);
        } else if (auto move_instr = dynamic_cast<assem::MoveInstr*>(*instr_it)) {
            RewriteMoveInstr(move_instr, v_temp, v_access, instr_it);
        }
    }
}

void RegAllocator::RewriteOperInstr(assem::OperInstr* oper_instr, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it) {
    RewriteUseDef(oper_instr->Use(), oper_instr->src_, v_temp, v_access, instr_it, true);
    RewriteUseDef(oper_instr->Def(), oper_instr->dst_, v_temp, v_access, instr_it, false);
}

void RegAllocator::RewriteMoveInstr(assem::MoveInstr* move_instr, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it) {
    RewriteUseDef(move_instr->Use(), move_instr->src_, v_temp, v_access, instr_it, true);
    RewriteUseDef(move_instr->Def(), move_instr->dst_, v_temp, v_access, instr_it, false);
}

void RegAllocator::RewriteUseDef(temp::TempList* use_def_list, temp::TempList*& src_dst_list, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it, bool is_use) {
    if (use_def_list != nullptr) {
        auto temp_list = use_def_list->GetList();
        if (std::find(temp_list.begin(), temp_list.end(), v_temp) != temp_list.end()) {
            auto replace_temp = temp::TempFactory::NewTemp();
            src_dst_list = temp::TempList::RewriteTempList(use_def_list, v_temp, replace_temp);

            std::string instr_assem = is_use ?
                "movq (" + frame_name_ + "_framesize_local" + std::to_string(v_access) + ")(`s0), `d0" :
                "movq `s0, (" + frame_name_ + "_framesize_local" + std::to_string(v_access) + ")(`s1)";

            auto src_list = new temp::TempList();
            auto dst_list = is_use ? new temp::TempList() : nullptr;

            if (is_use) {
                src_list->Append(reg_manager->Rsp());
                dst_list->Append(replace_temp);
            } else {
                src_list->Append(replace_temp);
                src_list->Append(reg_manager->Rsp());
            }

            assem::Instr* new_instr = new assem::OperInstr(
                instr_assem,
                dst_list,
                src_list,
                nullptr
            );

            if (is_use) {
                assem_instr_->GetInstrList()->Insert(instr_it, new_instr);
            } else {
                assem_instr_->GetInstrList()->Insert(std::next(instr_it), new_instr);
                ++instr_it; // Skip the newly inserted instruction
            }
        }
    }
}
} // namespace ra