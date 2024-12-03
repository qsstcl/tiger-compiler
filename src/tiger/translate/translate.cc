#include "tiger/translate/translate.h"

#include <asm-generic/errno-base.h>
#include <cassert>
#include <cstddef>
#include <llvm-14/llvm/IR/Constants.h>
#include <llvm-14/llvm/IR/DerivedTypes.h>
#include <memory>
#include <ostream>
#include <tiger/absyn/absyn.h>

#include "tiger/env/env.h"
#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/temp.h"
#include "tiger/frame/x64frame.h"
#include "tiger/semant/types.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <stack>
#include <vector>

extern frame::Frags *frags;
extern frame::RegManager *reg_manager;
extern llvm::IRBuilder<> *ir_builder;
extern llvm::Module *ir_module;
std::stack<llvm::Function *> func_stack;
std::stack<llvm::BasicBlock *> loop_stack;
llvm::Function *alloc_record;
llvm::Function *init_array;
llvm::Function *string_equal;
std::vector<std::pair<std::string, frame::Frame *>> frame_info;

bool CheckBBTerminatorIsBranch(llvm::BasicBlock *bb) {
  auto inst = bb->getTerminator();
  if (inst) {
    llvm::BranchInst *branchInst = llvm::dyn_cast<llvm::BranchInst>(inst);
    if (branchInst && !branchInst->isConditional()) {
      return true;
    }
  }
  return false;
}

int getActualFramesize(tr::Level *level) {
  return level->frame_->calculateActualFramesize();
}

namespace tr {

Access *Access::AllocLocal(Level *level, bool escape) {
  return new Access(level, level->frame_->AllocLocal(escape));
}

class ValAndTy {
public:
  type::Ty *ty_;
  llvm::Value *val_;
  llvm::BasicBlock *last_bb_;

  ValAndTy(llvm::Value *val, type::Ty *ty) : val_(val), ty_(ty) {}
};

void ProgTr::OutputIR(std::string_view filename) {
  std::string llvmfile = std::string(filename) + ".ll";
  std::error_code ec;
  llvm::raw_fd_ostream out(llvmfile, ec, llvm::sys::fs::OpenFlags::OF_Text);
  ir_module->print(out, nullptr);
}
void DeclareAllocRecordFunction() {
  llvm::LLVMContext &context = ir_builder->getContext();
  llvm::Module *module = ir_module;

  // 定义函数类型
  llvm::Type *int_type = llvm::Type::getInt32Ty(context);
  llvm::Type *int_ptr_type = llvm::Type::getInt32PtrTy(context);
  llvm::FunctionType *func_type =
      llvm::FunctionType::get(int_ptr_type, {int_type}, false);

  // 在模块中声明函数
  alloc_record = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, "alloc_record", module);
}
void ProgTr::Translate() {
  FillBaseVEnv();
  FillBaseTEnv();
  /* TODO: Put your lab5-part1 code here */
  DeclareAllocRecordFunction();
  absyn_tree_->Translate(venv_.get(), tenv_.get(), main_level_.get(),
                         errormsg_.get());
}

} // namespace tr

namespace absyn {

tr::ValAndTy *AbsynTree::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  return root_->Translate(venv, tenv, level, errormsg);
}

void TypeDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv, tr::Level *level,
                        err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto type_dec_list = types_->GetList();
  for (auto type_dec : type_dec_list) {
    type::Ty *res_ty = type_dec->ty_->Translate(tenv, errormsg);
    auto debug_ty = static_cast<type::RecordTy *>(res_ty);
    if (res_ty == nullptr) {
      errormsg->Error(pos_, "Translate failed");
      return;
    }
    tenv->Enter(type_dec->name_, new type::NameTy(type_dec->name_, res_ty));
  }
}

void FunctionDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                            tr::Level *level, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

void VarDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv, tr::Level *level,
                       err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto init_val_ty = init_->Translate(venv, tenv, level, errormsg);
  if (init_val_ty == NULL) {
    errormsg->Error(pos_, "Initilization failed");
    return;
  }
  auto init_ty = init_val_ty->ty_;
  tr::Access *access = tr::Access::AllocLocal(level, escape_);
  venv->Enter(var_, new env::VarEntry(access, init_ty));
}

type::Ty *NameTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

type::Ty *RecordTy::Translate(env::TEnvPtr tenv,
                              err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  std::vector<llvm::Type *> llvm_fields;
  type::FieldList *ty_fileds = new type::FieldList();
  for (auto field : record_->GetList()) {
    type::Ty *field_ty = tenv->Look(field->typ_);
    if (field_ty == nullptr) {
      errormsg->Error(pos_, "Undefined type");
      return nullptr;
    }
    type::Field *ty_field = new type::Field(field->name_, field_ty);
    llvm::Type *llvm_ty = field_ty->GetLLVMType();
    llvm_fields.push_back(llvm_ty);
    ty_fileds->Append(ty_field);
  }
  llvm::StructType *llvm_record_ty =
      llvm::StructType::create(llvm_fields, "MyStruct");

  return new type::RecordTy(ty_fileds);
}

type::Ty *ArrayTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *SimpleVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *FieldVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *SubscriptVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                      tr::Level *level,
                                      err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *VarExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *NilExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *IntExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::Value *val =
      llvm::ConstantInt::get(ir_builder->getInt32Ty(), val_, true);

  // 2. 返回包含值和类型的 ValAndTy
  return new tr::ValAndTy(val, type::IntTy::Instance());
}

tr::ValAndTy *StringExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *CallExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level,
                                 err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *OpExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *RecordExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  type::Ty *ty = tenv->Look(typ_);
  if (ty == nullptr) {
    errormsg->Error(pos_, "Undefined type");
    return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }
  type::RecordTy *record_ty = static_cast<type::RecordTy *>(ty);
  llvm::StructType *llvm_record_ty = llvm::cast<llvm::StructType>(
      record_ty->GetLLVMType()->getPointerElementType());
  std::cout << llvm_record_ty->getStructNumElements() * 8 << std::endl;
  llvm::Value *record_ptr = ir_builder->CreateCall(
      alloc_record,
      {llvm::ConstantInt::get(ir_builder->getInt32Ty(),
                              llvm_record_ty->getStructNumElements() * 8)});

  record_ptr =
      ir_builder->CreateIntToPtr(record_ptr, llvm_record_ty->getPointerTo());
  int index = 0;
  for (auto field : fields_->GetList()) {
    auto field_val_ty = field->exp_->Translate(venv, tenv, level, errormsg);
    if (field_val_ty == nullptr) {
      errormsg->Error(pos_, "Translate failed");
      return nullptr;
    }
    assert(field_val_ty->ty_ == record_ty->fields_->Get(index)->ty_);
    auto field_val = field_val_ty->val_;
    llvm::Value *field_ptr = ir_builder->CreateStructGEP(
        llvm_record_ty, record_ptr, index, field->name_->Name());
    ir_builder->CreateStore(field_val, field_ptr);
    index++;
  }

  record_ptr = ir_builder->CreatePtrToInt(
      record_ptr, llvm::Type::getInt64Ty(ir_builder->getContext()));

  return new tr::ValAndTy(record_ptr, record_ty);
}

tr::ValAndTy *SeqExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto exp_list = this->seq_->GetList();
  llvm::Value *last_val = nullptr;
  type::Ty *last_ty = nullptr;
  for (auto exp : exp_list) {
    std::cout << "Actual type: " << typeid(*exp).name() << std::endl;
    auto val_ty = exp->Translate(venv, tenv, level, errormsg);
    if (val_ty == nullptr) {
      errormsg->Error(pos_, "Translate failed");
      return nullptr;
    }
    last_val = val_ty->val_;
    last_ty = val_ty->ty_;
  }
  return new tr::ValAndTy(last_val, last_ty);
}

tr::ValAndTy *AssignExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *IfExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *WhileExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  temp::Label *test_label = temp::LabelFactory::NewLabel();
  temp::Label *body_label = temp::LabelFactory::NewLabel();
  temp::Label *done_label = temp::LabelFactory::NewLabel();
  tr::ValAndTy *test = test_->Translate(venv, tenv, level, errormsg);
  tr::ValAndTy *body = body_->Translate(venv, tenv, level, errormsg);
}

tr::ValAndTy *ForExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *BreakExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *LetExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto decs_list = this->decs_->GetList();
  for (auto dec : decs_list) {
    dec->Translate(venv, tenv, level, errormsg);
  }
  std::cout << "Actual type: " << typeid(*body_).name() << std::endl;
  return body_->Translate(venv, tenv, level, errormsg);
}

tr::ValAndTy *ArrayExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

tr::ValAndTy *VoidExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level,
                                 err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
}

} // namespace absyn