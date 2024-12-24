#ifndef TIGER_CODEGEN_CODEGEN_H_
#define TIGER_CODEGEN_CODEGEN_H_

#include "tiger/canon/canon.h"
#include "tiger/codegen/assem.h"
#include "tiger/frame/x64frame.h"
#include <cassert>

// Forward Declarations
namespace frame {
class RegManager;
class Frame;
} // namespace frame

namespace assem {
class Instr;
class InstrList;
} // namespace assem

namespace canon {
class Traces;
} // namespace canon

namespace cg {

class AssemInstr {
public:
  AssemInstr() = delete;
  explicit AssemInstr(assem::InstrList *instr_list) : instr_list_(instr_list) {}

  void Print(FILE *out, temp::Map *map) const;

  [[nodiscard]] assem::InstrList *GetInstrList() const { return instr_list_; }

private:
  assem::InstrList *instr_list_;
};

class CodeGen {
public:
  CodeGen(std::unique_ptr<canon::Traces> traces)
      : traces_(std::move(traces)), phi_temp_(temp::TempFactory::NewTemp()) {}

  void Codegen();

  // check if the value is %sp in llvm
  bool IsRsp(llvm::Value *val, std::string_view function_name) const {
    // TODO: your lab5 code here
    if(!val->hasName()){
      return false;
    }
    auto val_name = val->getName().str();
    auto func_str_name = function_name.data() + std::string("_sp");
    if(val_name == func_str_name){
      return true;
    }
    return false;
  }

  // bb is to add move instruction to record which block it jumps from
  // function_name can be used to construct return or exit label
  void InstrSel(assem::InstrList *instr_list, llvm::Instruction &inst,
                std::string_view function_name, llvm::BasicBlock *bb);
  std::unique_ptr<AssemInstr> TransferAssemInstr() {
    return std::move(assem_instr_);
  }

private:
  // record mapping from llvm value to temp
  std::unordered_map<llvm::Value *, temp::Temp *> *temp_map_;
  // for phi node, record mapping from llvm basic block to index of the block,
  // to check which block it jumps from
  std::unordered_map<llvm::BasicBlock *, int> *bb_map_;
  // for phi node, use a temp to record which block it jumps from
  temp::Temp *phi_temp_;
  std::unique_ptr<canon::Traces> traces_;
  std::unique_ptr<AssemInstr> assem_instr_;
};

} // namespace cg
#endif