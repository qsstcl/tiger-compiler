#ifndef TIGER_REGALLOC_REGALLOC_H_
#define TIGER_REGALLOC_REGALLOC_H_

#include "tiger/codegen/assem.h"
#include "tiger/codegen/codegen.h"
#include "tiger/frame/frame.h"
#include "tiger/frame/temp.h"
#include "tiger/liveness/liveness.h"
#include "tiger/regalloc/color.h"
#include "tiger/util/graph.h"

namespace ra {

class Result {
public:
  temp::Map *coloring_;
  assem::InstrList *il_;

  Result() : coloring_(nullptr), il_(nullptr) {}
  Result(temp::Map *coloring, assem::InstrList *il)
      : coloring_(coloring), il_(il) {}
  Result(const Result &result) = delete;
  Result(Result &&result) = delete;
  Result &operator=(const Result &result) = delete;
  Result &operator=(Result &&result) = delete;
  ~Result(){}
};

class RegAllocator {
  /* TODO: Put your lab6 code here */
public:
  std::string frame_name_;
  std::unique_ptr<cg::AssemInstr> assem_instr_;
  col::Color* color_;
  temp::Map* coloring_;

  RegAllocator(std::string frame_name, std::unique_ptr<cg::AssemInstr> assem_instr);
  ~RegAllocator();
  void RegAlloc();
  std::unique_ptr<ra::Result> TransferResult();
  void RewriteInstructions(temp::Temp* v_temp, int v_access);
  void RewriteOperInstr(assem::OperInstr* oper_instr, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it);
  void RewriteMoveInstr(assem::MoveInstr* move_instr, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it);
  void RewriteUseDef(temp::TempList* use_def_list, temp::TempList*& src_dst_list, temp::Temp* v_temp, int v_access, std::list<assem::Instr *>::const_iterator instr_it, bool is_use);
  
  void RewriteProgram(live::INodeListPtr spilledNodes);
};

} // namespace ra

#endif