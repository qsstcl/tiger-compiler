#include "tiger/codegen/codegen.h"
#include "tiger/codegen/assem.h"
#include "tiger/frame/temp.h"
#include "tiger/frame/x64frame.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

extern frame::RegManager *reg_manager;
extern frame::Frags *frags;

namespace {

constexpr int maxlen = 1024;

} // namespace

namespace cg {

void CodeGen::Codegen() {
  temp_map_ = new std::unordered_map<llvm::Value *, temp::Temp *>();
  bb_map_ = new std::unordered_map<llvm::BasicBlock *, int>();
  auto *list = new assem::InstrList();

  // firstly get all global string's location
  for (auto &&frag : frags->GetList()) {
    if (auto *str_frag = dynamic_cast<frame::StringFrag *>(frag)) {
      auto tmp = temp::TempFactory::NewTemp();
      list->Append(new assem::OperInstr(
          "leaq " + std::string(str_frag->str_val_->getName()) + "(%rip),`d0",
          new temp::TempList(tmp), new temp::TempList(), nullptr));
      temp_map_->insert({str_frag->str_val_, tmp});
    }
  }

  // move arguments to temp
  auto arg_iter = traces_->GetBody()->arg_begin();
  auto regs = reg_manager->ArgRegs();
  auto tmp_iter = regs->GetList().begin();

  // first arguement is rsp, we need to skip it
  ++arg_iter;

  for (; arg_iter != traces_->GetBody()->arg_end() &&
         tmp_iter != regs->GetList().end();
       ++arg_iter, ++tmp_iter) {
    auto tmp = temp::TempFactory::NewTemp();
    list->Append(new assem::OperInstr("movq `s0,`d0", new temp::TempList(tmp),
                                      new temp::TempList(*tmp_iter), nullptr));
    temp_map_->insert({static_cast<llvm::Value *>(arg_iter), tmp});
  }

  // pass-by-stack parameters
  if (arg_iter != traces_->GetBody()->arg_end()) {
    auto last_sp = temp::TempFactory::NewTemp();
    list->Append(
        new assem::OperInstr("movq %rsp,`d0", new temp::TempList(last_sp),
                             new temp::TempList(reg_manager->GetRegister(
                                 frame::X64RegManager::Reg::RSP)),
                             nullptr));
    list->Append(new assem::OperInstr(
        "addq $" + std::string(traces_->GetFunctionName()) +
            "_framesize_local,`s0",
        new temp::TempList(last_sp),
        new temp::TempList({last_sp, reg_manager->GetRegister(
                                         frame::X64RegManager::Reg::RSP)}),
        nullptr));
    while (arg_iter != traces_->GetBody()->arg_end()) {
      auto tmp = temp::TempFactory::NewTemp();
      list->Append(new assem::OperInstr(
          "movq " +
              std::to_string(8 * (arg_iter - traces_->GetBody()->arg_begin())) +
              "(`s0),`d0",
          new temp::TempList(tmp), new temp::TempList(last_sp), nullptr));
      temp_map_->insert({static_cast<llvm::Value *>(arg_iter), tmp});
      ++arg_iter;
    }
  }

  // construct bb_map
  int bb_index = 0;
  for (auto &&bb : traces_->GetBasicBlockList()->GetList()) {
    bb_map_->insert({bb, bb_index++});
  }

  //把每一条llvm instruction 得到的value 转成一个temp
  for (auto &&bb : traces_->GetBasicBlockList()->GetList()) {
    // record every return value from llvm instruction
    for (auto &&inst : bb->getInstList())
      temp_map_->insert({&inst, temp::TempFactory::NewTemp()});
  }

  for (auto &&bb : traces_->GetBasicBlockList()->GetList()) {
    // Generate label for basic block
    list->Append(new assem::LabelInstr(std::string(bb->getName())));

    // Generate instructions for basic block
    for (auto &&inst : bb->getInstList())
      InstrSel(list, inst, traces_->GetFunctionName(), bb);
  }

  assem_instr_ = std::make_unique<AssemInstr>(frame::ProcEntryExit2(
      frame::ProcEntryExit1(traces_->GetFunctionName(), list)));
}

void AssemInstr::Print(FILE *out, temp::Map *map) const {
  for (auto instr : instr_list_->GetList())
    instr->Print(out, map);
  fprintf(out, "\n");
}

void CodeGen::InstrSel(assem::InstrList *instr_list, llvm::Instruction &inst,
                       std::string_view function_name, llvm::BasicBlock *bb) {
  // TODO: your lab5 code here
  std::string inst_name = std::string(inst.getOpcodeName()) ;
  auto current_inst = inst.getOpcode();
  if(current_inst == llvm::Instruction::Load){
    auto load_inst = llvm::dyn_cast<llvm::LoadInst>(&inst);
    auto pointer_value= load_inst->getPointerOperand();
    if (llvm::isa<llvm::GlobalVariable>(pointer_value)) {
        // Handle global variable
        auto global_var = llvm::dyn_cast<llvm::GlobalVariable>(pointer_value);
        auto global_name = global_var->getName();
        auto load_dest = (*temp_map_)[&inst];
        instr_list->Append(new assem::OperInstr(
            "movq " + global_name.str() + "(%rip), `d0",
            new temp::TempList(load_dest),
            nullptr,
            nullptr));
    }else {
        // Handle local variable
        auto load_src = (*temp_map_)[pointer_value];
        auto load_dest = (*temp_map_)[&inst];
        instr_list->Append(new assem::OperInstr(
            "movq (`s0), `d0",
            new temp::TempList(load_dest),
            new temp::TempList(load_src),
            nullptr));
    }
  }else if(current_inst == llvm::Instruction::Sub){
    auto binary_inst = llvm::dyn_cast<llvm::SubOperator>(&inst);
    auto dst_name = binary_inst->getName();
    if(IsRsp(binary_inst, function_name)){
      return;
    }else{
      auto first_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(0));
      auto second_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(1));
      auto dst = (*temp_map_)[&inst];
      if(llvm::isa<llvm::ConstantInt>(first_operand)){
        auto constant = llvm::dyn_cast<llvm::ConstantInt>(first_operand);
        auto constant_value = constant->getSExtValue();
        auto src = (*temp_map_)[second_operand];
        instr_list->Append(new assem::OperInstr(
          "movq $" + std::to_string(constant_value) + ", `d0",
          new temp::TempList(dst),
          nullptr,
          nullptr));
        instr_list->Append(new assem::OperInstr(
          "subq `s0, `d0",
          new temp::TempList(dst),
          new temp::TempList(src),
          nullptr));
        return;
      }
      if(llvm::isa<llvm::ConstantInt>(second_operand)){
        auto constant = llvm::dyn_cast<llvm::ConstantInt>(second_operand);
        auto constant_value = constant->getSExtValue();
        auto src = (*temp_map_)[first_operand];
        instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",
          new temp::TempList(dst),
          new temp::TempList(src),
          nullptr
        ));
        instr_list->Append(new assem::OperInstr(
          "subq $" + std::to_string(constant_value) + ", `d0",
          new temp::TempList(dst),
          nullptr,
          nullptr));
        return;
      }
      auto src1 = (*temp_map_)[first_operand];
      auto src2 = (*temp_map_)[second_operand];
      instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",
          new temp::TempList(dst),
          new temp::TempList(src1),
          nullptr));
      instr_list->Append(new assem::OperInstr(
          "subq `s0, `d0",
          new temp::TempList(dst),
          new temp::TempList(src2),
          nullptr));
    }
  }else if(current_inst == llvm::Instruction::Add){
    auto binary_inst = llvm::dyn_cast<llvm::AddOperator>(&inst);
    auto first_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(0));
    auto second_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(1));
    auto dst = (*temp_map_)[&inst];
    if(IsRsp(first_operand, function_name)){
      auto dest = (*temp_map_)[&inst];
      auto src = (*temp_map_)[second_operand];
      instr_list->Append(new assem::OperInstr(
        "movq `s0, `d0",
        new temp::TempList(dest),
        new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RSP)),
        nullptr));

      instr_list->Append(new assem::OperInstr(
          "addq `s0, `d0",
          new temp::TempList(dest),
          new temp::TempList(src),
          nullptr));
      return;
    }
    if(llvm::isa<llvm::ConstantInt>(first_operand)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(first_operand);
      auto constant_value = constant->getSExtValue();
      auto src = (*temp_map_)[second_operand];
      instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",                                     
          new temp::TempList(dst),                              
          new temp::TempList(src),                          
          nullptr));
      instr_list->Append(new assem::OperInstr(
          "addq $" + std::to_string(constant_value) + ", `d0", 
          new temp::TempList(dst),
          new temp::TempList({dst}),                             // 使用源寄存器
          nullptr));
      return ;
    }

    if(llvm::isa<llvm::ConstantInt>(second_operand)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(second_operand);
      auto constant_value = constant->getSExtValue();
      auto src = (*temp_map_)[first_operand];
      instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",                                     
          new temp::TempList(dst),                              
          new temp::TempList({src}),                          
          nullptr));
      instr_list->Append(new assem::OperInstr(
          "addq $" + std::to_string(constant_value) + ", `d0", 
          new temp::TempList(dst),
          new temp::TempList({dst}),                             // 使用源寄存器
          nullptr));

      // 将中间结果从 s0 移到 d0
      return;
    }
    auto src1 = (*temp_map_)[first_operand];
    auto src2 = (*temp_map_)[second_operand];
    instr_list->Append(new assem::OperInstr(
      "movq `s0, `d0",
      new temp::TempList(dst),
      new temp::TempList(src1),
      nullptr));
    instr_list->Append(new assem::OperInstr(
        "addq `s1, `d0",
        new temp::TempList(dst),
        new temp::TempList({src1, src2}),
        nullptr));
    
  }else if(current_inst == llvm::Instruction::Mul){
    auto binary_inst = llvm::dyn_cast<llvm::BinaryOperator>(&inst);
    auto first_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(0));
    auto second_operand = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(1));
    auto dst = (*temp_map_)[&inst];
    if(llvm::isa<llvm::ConstantInt>(second_operand)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(second_operand);
      auto constant_value = constant->getSExtValue();
      auto src = (*temp_map_)[first_operand];

      // 将中间结果从 s0 移到 d0
      instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",                                      // 从 s0 移到 d0
          new temp::TempList(dst),                               // 目标寄存器是 d0
          new temp::TempList({src}),                             // 使用源寄存器
          nullptr));
      // 先计算 constant_value * s0，将结果存储到一个临时寄存器（s0）
      instr_list->Append(new assem::OperInstr(
          "imulq $" + std::to_string(constant_value) + ", `d0",  // 使用 s0 作为源寄存器
          new temp::TempList(dst),                               // 将结果存储到 d0
          new temp::TempList({src}),                              // 使用源寄存器
          nullptr));
      return;
    }
    auto src1 = (*temp_map_)[first_operand];
    auto src2 = (*temp_map_)[second_operand];
    instr_list->Append(new assem::OperInstr(
        "movq `s0, `d0",
        new temp::TempList(dst),
        new temp::TempList(src2),
        nullptr));
    instr_list->Append(new assem::OperInstr(
        "imulq `s0, `d0",
        new temp::TempList(dst),
        new temp::TempList(src1),
        nullptr));
  }else if(current_inst == llvm::Instruction::SDiv){
    auto binary_inst = llvm::dyn_cast<llvm::BinaryOperator>(&inst);

    auto dividend = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(0));
    auto divisor = llvm::dyn_cast<llvm::Value>(binary_inst->getOperand(1));
    auto dst = (*temp_map_)[&inst];
    auto src1 = (*temp_map_)[dividend];

    instr_list->Append(new assem::OperInstr(
      "movq `s0, %rax",
      new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RAX)),
      new temp::TempList(src1),
      nullptr));
    
    instr_list->Append(new assem::OperInstr(
      "cqto",
      new temp::TempList({reg_manager->GetRegister(frame::X64RegManager::RAX), reg_manager->GetRegister(frame::X64RegManager::RDX)}),
      nullptr,
      nullptr));
    if(llvm::isa<llvm::ConstantInt>(divisor)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(divisor);
      auto constant_value = constant->getSExtValue();
      instr_list->Append(new assem::OperInstr(
        "idivq $" + std::to_string(constant_value),
        new temp::TempList({reg_manager->GetRegister(frame::X64RegManager::RAX), reg_manager->GetRegister(frame::X64RegManager::RDX)}),
        nullptr,
        nullptr));
    }else{
      auto src2 = (*temp_map_)[divisor];
      instr_list->Append(new assem::OperInstr(
        "idivq `s0",
        new temp::TempList({reg_manager->GetRegister(frame::X64RegManager::RAX), reg_manager->GetRegister(frame::X64RegManager::RDX)}),
        new temp::TempList(src2),
        nullptr));
    }
    instr_list->Append(new assem::OperInstr(
    "movq %rax, `d0",
    new temp::TempList(dst),
    new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RAX)),
    nullptr));
  }else if(current_inst == llvm::Instruction::PtrToInt){
    auto ptrtoint_inst = llvm::dyn_cast<llvm::PtrToIntOperator>(&inst);

    llvm::Value* pointer_operand = ptrtoint_inst->getOperand(0);
    auto dest_temp = (*temp_map_)[&inst];
    auto src_temp = (*temp_map_)[pointer_operand];
    instr_list->Append(new assem::OperInstr(
      "movq `s0, `d0",
      new temp::TempList(dest_temp),
      new temp::TempList(src_temp),
      nullptr));
  }else if(current_inst == llvm::Instruction::IntToPtr){
    auto inttoptr_inst = llvm::dyn_cast<llvm::IntToPtrInst>(&inst);

    llvm::Value* int_operand = inttoptr_inst->getOperand(0);
    auto dest_temp = (*temp_map_)[&inst];
    auto src_temp = (*temp_map_)[int_operand];
    instr_list->Append(new assem::OperInstr(
      "movq `s0, `d0",
      new temp::TempList(dest_temp),
      new temp::TempList(src_temp),
      nullptr));
  }else if(current_inst == llvm::Instruction::GetElementPtr){
    auto gep_inst = llvm::dyn_cast<llvm::GetElementPtrInst>(&inst);
    auto index_temp = temp::TempFactory::NewTemp();
    instr_list->Append(new assem::MoveInstr(
      "movq `s0, `d0",
      new temp::TempList((*temp_map_)[&inst]),
      new temp::TempList((*temp_map_)[gep_inst->getPointerOperand()])
    ));
    if (gep_inst->getNumOperands() == 2){
      auto index = gep_inst->getOperand(1);
      instr_list->Append(new assem::OperInstr(
        "movq `s0, `d0",
        new temp::TempList(index_temp),
        new temp::TempList((*temp_map_)[index]),
        nullptr));

      instr_list->Append(new assem::OperInstr(
        "imulq $" + std::to_string(8) + ", `d0",
        new temp::TempList(index_temp),
        nullptr,
        nullptr));
      
      instr_list->Append(new assem::OperInstr(
        "addq `s0, `d0",
        new temp::TempList((*temp_map_)[&inst]),
        new temp::TempList({index_temp}),
        nullptr));
      return;
    }else{
      auto index = gep_inst->getOperand(2);
      if(llvm::isa<llvm::ConstantInt>(index)){
        auto constant = llvm::dyn_cast<llvm::ConstantInt>(index);
        auto constant_value = constant->getSExtValue();
        instr_list->Append(new assem::OperInstr(
          "movq $" + std::to_string(constant_value) + ", `d0",
          new temp::TempList(index_temp),
          nullptr,
          nullptr));
        
      }else{
        instr_list->Append(new assem::OperInstr(
          "movq `s0, `d0",
          new temp::TempList(index_temp),
          new temp::TempList((*temp_map_)[index]),
          nullptr));
      }
      instr_list->Append(new assem::OperInstr(
        "imulq $" + std::to_string(8) + ", `d0",
        new temp::TempList(index_temp),
        nullptr,
        nullptr));

      instr_list->Append(new assem::OperInstr(
        "addq `s0, `d0",
        new temp::TempList((*temp_map_)[&inst]),
        new temp::TempList({index_temp}),
        nullptr));
      return;
    }
  }else if(current_inst == llvm::Instruction::Store){
    auto store_inst = llvm::dyn_cast<llvm::StoreInst>(&inst);

    auto value = store_inst->getValueOperand();
    auto pointer = store_inst->getPointerOperand();
    if(llvm::isa<llvm::ConstantInt>(value)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(value);
      auto constant_value = constant->getSExtValue();
      auto dst = (*temp_map_)[pointer];
      instr_list->Append(new assem::OperInstr(
        "movq $" + std::to_string(constant_value) + ", (`s0)",
        nullptr,
        new temp::TempList(dst),
        nullptr));
    }else{
      auto src = (*temp_map_)[value];
      auto dst = (*temp_map_)[pointer];
      instr_list->Append(new assem::OperInstr(
        "movq `s0, (`s1)",
        nullptr,
        new temp::TempList({src, dst}),
        nullptr));
    }
    
  }else if(current_inst == llvm::Instruction::ZExt){
    auto zext_inst = llvm::dyn_cast<llvm::ZExtOperator>(&inst);

    auto value = zext_inst->getOperand(0);
    auto src = (*temp_map_)[value];
    auto dst = (*temp_map_)[&inst];
    instr_list->Append(new assem::OperInstr(
      "movzx `s0, `d0",
      new temp::TempList(dst),
      new temp::TempList(src),
      nullptr));
  }else if(current_inst == llvm::Instruction::Call){
    auto call_inst = llvm::dyn_cast<llvm::CallInst>(&inst);

    //获取被调用函数的名字
    auto called_function = call_inst->getCalledFunction();
    if(!called_function){
      throw std::runtime_error("Unsupported call instruction");
    }

    auto func_name = called_function->getName();
    auto args_num = call_inst->arg_size();
    auto pass_arg_registers = reg_manager->ArgRegs()->GetList();
    auto pass_arg_registers_iter = pass_arg_registers.begin();
    for(int i = 0; i < args_num; i++){
      auto arg = call_inst->getArgOperand(i);
      if(i == 0 && IsRsp(arg, function_name)){
        continue;
      }

      if(llvm::isa<llvm::ConstantInt>(arg)){
        auto constant = llvm::dyn_cast<llvm::ConstantInt>(arg);
        auto constant_value = constant->getSExtValue();
        auto dst = *pass_arg_registers_iter;
        instr_list->Append(new assem::OperInstr(
          "movq $" + std::to_string(constant_value) + ", `d0",
          new temp::TempList(dst),
          nullptr,
          nullptr));
        pass_arg_registers_iter++;
        continue;
      }
      if(IsRsp(arg, function_name)){
        instr_list->Append(new assem::OperInstr(
          "movq %rsp, `d0",
          new temp::TempList(*pass_arg_registers_iter),
          nullptr,
          nullptr));
        pass_arg_registers_iter++;
        continue;
      }
      auto src = (*temp_map_)[arg];
      auto dst = *pass_arg_registers_iter;
      instr_list->Append(new assem::OperInstr(
        "movq `s0, `d0",
        new temp::TempList(dst),
        new temp::TempList(src),
        nullptr));
      pass_arg_registers_iter++;
    }
    instr_list->Append(new assem::OperInstr(
      "call " + func_name.str(),
      reg_manager->CallerSaves(),
      nullptr,
      nullptr));

    if(!call_inst->getType()->isVoidTy()){
      auto dst = (*temp_map_)[&inst];
      instr_list->Append(new assem::OperInstr(
        "movq %rax, `d0",
        new temp::TempList(dst),
        new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RAX)),
        nullptr));
    }
  }else if(current_inst == llvm::Instruction::Ret){
    auto ret_inst = llvm::dyn_cast<llvm::ReturnInst>(&inst);
    if(ret_inst->getNumOperands() > 0){
      llvm::Value *ret_value = ret_inst->getOperand(0);
      if(llvm::isa<llvm::ConstantInt>(ret_value)){
        auto constant = llvm::dyn_cast<llvm::ConstantInt>(ret_value);
        auto constant_value = constant->getSExtValue();
        instr_list->Append(new assem::OperInstr(
          "movq $" + std::to_string(constant_value) + ", %rax",
          new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RAX)),
          nullptr,
          nullptr));
      }else{
        auto src = (*temp_map_)[ret_value];
        instr_list->Append(new assem::OperInstr(
          "movq `s0, %rax",
          new temp::TempList(reg_manager->GetRegister(frame::X64RegManager::RAX)),
          new temp::TempList(src),
          nullptr));
      }
    }
    instr_list->Append(new assem::OperInstr(
      "jmp " + std::string(function_name) + "_end",
      nullptr,
      nullptr,
      nullptr));
  }else if(current_inst == llvm::Instruction::Br){
    auto br_inst = llvm::dyn_cast<llvm::BranchInst>(&inst);
    auto current_bb = inst.getParent();
    auto it = bb_map_->find(current_bb);
    auto jmp_temp_reg = temp::TempFactory::NewTemp();
    if (it != bb_map_->end()) {
        int bb_index = it->second;
        // 将编号存储到 %rax
        instr_list->Append(new assem::OperInstr(
            "movq $" + std::to_string(bb_index) + ", %rax",
            nullptr,
            nullptr,
            nullptr
        ));
    }
    if(br_inst->isConditional()){
      llvm::Value *cond = br_inst->getCondition();
      auto cond_temp = (*temp_map_)[cond];
      instr_list->Append(new assem::OperInstr(
        "cmpq $0, `s0",
        nullptr,
        new temp::TempList(cond_temp),
        nullptr));
      instr_list->Append(new assem::OperInstr(
        "jne " + std::string(br_inst->getSuccessor(0)->getName()),
        nullptr,
        nullptr,
        nullptr));
      instr_list->Append(new assem::OperInstr(
        "jmp " + std::string(br_inst->getSuccessor(1)->getName()),
        nullptr,
        nullptr,
        nullptr));
      
    }else{
      
      auto target_block_name = br_inst->getSuccessor(0)->getName();
      instr_list->Append(new assem::OperInstr(
        "jmp " + std::string(target_block_name),
        nullptr,
        nullptr,
        nullptr));
    }
  }else if(current_inst == llvm::Instruction::ICmp){
    auto icmp_inst = llvm::dyn_cast<llvm::ICmpInst>(&inst);

    // 获取操作数
    auto lhs = icmp_inst->getOperand(0);
    auto rhs = icmp_inst->getOperand(1);
    auto lhs_temp = (*temp_map_)[lhs];
    auto dest_temp = (*temp_map_)[&inst];  // 存储比较结果

    if(llvm::isa<llvm::ConstantInt>(rhs)){
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(rhs);
      auto constant_value = constant->getSExtValue();
      // 生成 cmpq 指令
      instr_list->Append(new assem::OperInstr(
          "cmpq $" + std::to_string(constant_value) + ", `s0",
          nullptr,
          new temp::TempList(lhs_temp),
          nullptr));
      
    }else{
      auto rhs_temp = (*temp_map_)[rhs];
      // 生成 cmpq 指令
      instr_list->Append(new assem::OperInstr(
          "cmpq `s1, `s0",
          nullptr,
          new temp::TempList({lhs_temp, rhs_temp}),
          nullptr));

    }

    // 根据比较类型生成相应的 setX 指令
    std::string set_instr;
    switch (icmp_inst->getPredicate()) {
        case llvm::CmpInst::ICMP_EQ:
            set_instr = "sete";  // Equal
            break;
        case llvm::CmpInst::ICMP_NE:
            set_instr = "setne";  // Not Equal
            break;
        case llvm::CmpInst::ICMP_SLT:
            set_instr = "setl";  // Signed Less Than
            break;
        case llvm::CmpInst::ICMP_SLE:
            set_instr = "setle";  // Signed Less or Equal
            break;
        case llvm::CmpInst::ICMP_SGT:
            set_instr = "setg";  // Signed Greater Than
            break;
        case llvm::CmpInst::ICMP_SGE:
            set_instr = "setge";  // Signed Greater or Equal
            break;
        default:
            throw std::runtime_error("Unsupported ICmp predicate");
    }

    // 生成 setX 指令
    instr_list->Append(new assem::OperInstr(
        set_instr + " `d0",
        new temp::TempList(dest_temp),  // 设置目标寄存器
        nullptr,
        nullptr));

  }else if(current_inst == llvm::Instruction::PHI){
    auto phi_inst = llvm::dyn_cast<llvm::PHINode>(&inst);
    auto dest_temp = (*temp_map_)[&inst];
    auto temp_register = temp::TempFactory::NewTemp();
    // 创建结束标签
    auto end_label = temp::LabelFactory::NewLabel();
    std::list<temp::Label *> label_list ;
    for (unsigned i = 0; i < phi_inst->getNumIncomingValues(); i++) {
      auto incoming_value = phi_inst->getIncomingValue(i);
      auto incoming_block = phi_inst->getIncomingBlock(i);

      // 获取分支基本块的编号
      auto it = bb_map_->find(incoming_block);
      assert(it != bb_map_->end());
      int bb_index = it->second;

      // 创建唯一标签
      auto unique_label = temp::LabelFactory::NewLabel();
      label_list.push_back(unique_label);
      // 比较 %rax 和分支编号
      instr_list->Append(new assem::OperInstr(
          "cmpq $" + std::to_string(bb_index) + ", %rax",
          nullptr,
          nullptr,
          nullptr
      ));

      instr_list->Append(new assem::OperInstr(
          "je " + unique_label->Name(),
          nullptr,
          nullptr,
          nullptr
      ));

    }
    for(unsigned i = 0; i < phi_inst->getNumIncomingValues(); i++){
      auto incoming_value = phi_inst->getIncomingValue(i);
      auto incoming_block = phi_inst->getIncomingBlock(i);
      auto unique_label = label_list.front();
      label_list.pop_front();
      // 在 unique_label 中移动值到目标寄存器
      instr_list->Append(new assem::LabelInstr(unique_label->Name()));
      if (llvm::isa<llvm::ConstantInt>(incoming_value)) {
          auto constant = llvm::dyn_cast<llvm::ConstantInt>(incoming_value);
          auto constant_value = constant->getSExtValue();
          instr_list->Append(new assem::OperInstr(
              "movq $" + std::to_string(constant_value) + ", `d0",
              new temp::TempList(dest_temp),
              nullptr,
              nullptr
          ));
      } else {

        auto src_temp = (*temp_map_)[incoming_value];
        if (src_temp == nullptr) {
          // 如果 incoming_value 为空，生成默认值或添加警告
          instr_list->Append(new assem::OperInstr(
              "movq $0, `d0", // 默认值设为 0
              new temp::TempList(dest_temp),
              nullptr,
              nullptr
          ));
        } else{
            instr_list->Append(new assem::MoveInstr(
                "movq `s0, `d0",
                new temp::TempList(dest_temp),
                new temp::TempList(src_temp)
                ));
        }
          
      }
      

      // 跳转到结束标签
      instr_list->Append(new assem::OperInstr(
          "jmp " + end_label->Name(),
          nullptr,
          nullptr,
          nullptr
      ));
    }
    // 添加结束标签
    instr_list->Append(new assem::LabelInstr(end_label->Name()));
  }else{
    throw std::runtime_error(std::string("Unknown instruction: ") +
                              inst.getOpcodeName());
  }

}

} // namespace cg