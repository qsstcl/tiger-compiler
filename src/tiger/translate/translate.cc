#include "tiger/translate/translate.h"

#include <asm-generic/errno-base.h>
#include <cassert>
#include <cstddef>
#include <list>
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
#include "tiger/frame/frame.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/GlobalVariable.h"
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
#include <utility>
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
llvm::Type *string;
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
  llvm::Type *int_ptr_type = llvm::Type::getInt64Ty(context);
  llvm::FunctionType *func_type =
      llvm::FunctionType::get(int_ptr_type, {int_type}, false);

  // 在模块中声明函数
  alloc_record = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, "alloc_record", module);
}
void DeclareInitArrayFunction(){
  llvm::LLVMContext &context = ir_builder->getContext();
  llvm::Module *module = ir_module;

  // 定义函数类型
  llvm::Type *size_type = llvm::Type::getInt32Ty(context);
  llvm::Type *init_type = llvm::Type::getInt64Ty(context);
  llvm::Type *int64type = llvm::Type::getInt64Ty(context);
  llvm::FunctionType *func_type =
      llvm::FunctionType::get(int64type, {size_type,init_type}, false);

  // 在模块中声明函数
  init_array = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, "init_array", module);
}
void DeclareStringEqual(){
  llvm::LLVMContext &context = ir_builder->getContext();
  llvm::Module *module = ir_module;

  string = type::StringTy::Instance()->GetLLVMType();

  string_equal = llvm::Function::Create(
      llvm::FunctionType::get(llvm::Type::getInt1Ty(context), {string,string}, false),
      llvm::Function::ExternalLinkage, "string_equal", module);
}
void ProgTr::Translate() {
  FillBaseVEnv();
  FillBaseTEnv();
  /* TODO: Put your lab5-part1 code here */
  DeclareAllocRecordFunction();
  DeclareInitArrayFunction();
  DeclareStringEqual();
  auto tiger_main_type = llvm::FunctionType::get(ir_builder->getInt32Ty(),{ir_builder->getInt64Ty(), ir_builder->getInt64Ty()}, false);
  llvm::Function *tiger_main = llvm::Function::Create(tiger_main_type,
                                                      llvm::Function::ExternalLinkage,
                                                      "tigermain", ir_module);
  llvm::GlobalVariable *tigermain_framesize_global = new llvm::GlobalVariable(
      *ir_module, ir_builder->getInt64Ty(), true,
      llvm::GlobalValue::ExternalLinkage, nullptr, "tigermain_framesize");
  main_level_->frame_->framesize_global = tigermain_framesize_global;
  llvm::BasicBlock *entry = llvm::BasicBlock::Create(ir_builder->getContext(), "tigermain", tiger_main);
  ir_builder->SetInsertPoint(entry);
  llvm::Value *sp_arg_val = tiger_main->getArg(0);
  llvm::Value *function_framesize = ir_builder->CreateLoad(ir_builder->getInt64Ty(),tigermain_framesize_global);
  llvm::Value *new_sp = ir_builder->CreateSub(sp_arg_val, function_framesize);
  main_level_->set_sp(new_sp);
  func_stack.push(tiger_main);
  frame_info.push_back(std::make_pair(tiger_main->getName().str(), main_level_->frame_));
  absyn_tree_->Translate(venv_.get(), tenv_.get(), main_level_.get(),
                         errormsg_.get());
  auto main_framesize =  main_level_->frame_->calculateActualFramesize();
  tigermain_framesize_global->setInitializer(llvm::ConstantInt::get(ir_builder->getInt64Ty(), main_framesize));
  ir_builder->CreateRet(ir_builder->getInt32(0));
  func_stack.pop();
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
  // First pass: Check for duplicate type names and initialize NameTy placeholders.
  for (auto type_entry : types_->GetList()) {
    if (tenv->Look(type_entry->name_)) {
      assert(false);
    }
    tenv->Enter(type_entry->name_, new type::NameTy(type_entry->name_, nullptr));
  }
  // Second pass: Analyze types and resolve placeholders.
  for (auto type_entry : types_->GetList()) {
    type::Ty *resolved_ty = type_entry->ty_->SemAnalyze(tenv, errormsg);
    // Replace NameTy with resolved type unless it remains unresolved.
    if (typeid(*resolved_ty) != typeid(type::NameTy)) {
      tenv->Set(type_entry->name_, resolved_ty);
    }
    // Special handling for RecordTy: Resolve self-references in field types.
    if (auto *record_ty = dynamic_cast<type::RecordTy *>(resolved_ty)) {
      for (type::Field *field : record_ty->fields_->GetList()) {
        if (field->ty_ && typeid(*field->ty_) == typeid(type::NameTy) && static_cast<type::NameTy *>(field->ty_)->sym_->Name() == type_entry->name_->Name()) {
          field->ty_ = resolved_ty;
        }
      }
    }
  }
  // Third pass: Detect type cycles and finalize type resolution.
  for (auto type_entry : types_->GetList()) {
    type::Ty *current_ty = tenv->Look(type_entry->name_);
    std::list<type::NameTy *> unresolved_name_tys;
    // Traverse the definition chain to detect cycles.
    while (auto *name_ty = dynamic_cast<type::NameTy *>(current_ty)) {
      if (std::find(unresolved_name_tys.begin(), unresolved_name_tys.end(), name_ty) != unresolved_name_tys.end()) {
        assert(false);
      }
      unresolved_name_tys.push_back(name_ty);
      current_ty = name_ty->sym_->Name() == type_entry->name_->Name() ? nullptr : tenv->Look(name_ty->sym_);
    }
    // Resolve NameTy placeholders to the actual type.
    for (type::NameTy *name_ty : unresolved_name_tys) {
      tenv->Set(name_ty->sym_, current_ty);
    }

  }
}
void FunctionDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                            tr::Level *level, err::ErrorMsg *errormsg) const {
  // prams_list 是函数本来的参数列表
  // 为了添加sp和sl的抽象，我们强制给所有的函数添加两个参数，第一个是调用它的函数的sp，第二个是它的static link

  /* TODO: Put your lab5-part1 code here */
  auto function_dec_list = this->functions_->GetList();
  for (auto funcDec : function_dec_list) {
    // std::cout << "Declaring function: " << funcDec->name_->Name() << std::endl;

    // 获取参数列表
    auto params_list = funcDec->params_->GetList();
    std::vector<llvm::Type *> llvm_param_types;
    type::TyList *formals_typelist = new type::TyList();
    // 函数的第一个参数是调用它的函数的sp，第二个参数是它的static link，我们先把这两个参数加入到参数列表中
    // 遍历参数并收集LLVM参数类型
    for (auto param : params_list) {
      auto type_entry = tenv->Look(param->typ_);
      if (!type_entry) {
        errormsg->Error(funcDec->pos_, "undefined type: %s", param->typ_->Name().c_str());
        return;
      }
      formals_typelist->Append(type_entry);
    }
    type::Ty *result_ty = nullptr;
    // 返回值类型
    llvm::Type *llvm_return_type = llvm::Type::getVoidTy(ir_builder->getContext());
    if (funcDec->result_) {
      auto return_type_entry = tenv->Look(funcDec->result_);
      if (!return_type_entry) {
        errormsg->Error(funcDec->pos_, "undefined return type: %s", funcDec->result_->Name().c_str());
        return;
      }
      llvm_return_type = return_type_entry->ActualTy()->GetLLVMType();
      result_ty = return_type_entry;
    }

    // 创建新作用域和级别
    std::list<bool> escape_list(params_list.size(), true);
    auto new_level =
        new tr::Level(frame::NewFrame(temp::LabelFactory::NamedLabel(funcDec->name_->Name()), escape_list), level);
    llvm::GlobalVariable *new_func_framesize_global = new llvm::GlobalVariable(
        *ir_module, ir_builder->getInt64Ty(), true,
        llvm::GlobalValue::ExternalLinkage, nullptr, funcDec->name_->Name() + "_framesize");
    new_level->frame_->framesize_global = new_func_framesize_global;
    // std::string outfile = "temp.ll";
    // std::error_code EC;
    // llvm::raw_fd_ostream file(outfile, EC);
    // ir_module->print(file, nullptr);
    // 将函数声明添加到环境
    auto new_funcEntry = new env::FunEntry(new_level, formals_typelist, result_ty, funcDec->name_->Name());
    venv->Enter(funcDec->name_, new_funcEntry);
    auto llvm_function = new_funcEntry->func_;
    // std::string outfile = "temp.ll";
    // std::error_code EC;
    // llvm::raw_fd_ostream file(outfile, EC);
    // ir_module->print(file, nullptr);
    auto current_block = ir_builder->GetInsertBlock();
    // 创建函数体的基本块
    auto func_bb = llvm::BasicBlock::Create(ir_builder->getContext(), funcDec->name_->Name(), llvm_function);

    ir_builder->SetInsertPoint(func_bb);

    llvm::Value *sp_arg_val = llvm_function->getArg(0);
    llvm::Value *function_framesize = ir_builder->CreateLoad(ir_builder->getInt64Ty(),new_func_framesize_global,"local_framesize");
    llvm::Value *new_sp = ir_builder->CreateSub(sp_arg_val, function_framesize);
    new_level->set_sp(new_sp);
    function_framesize = ir_builder->CreateLoad(ir_builder->getInt64Ty(),new_func_framesize_global,"local_framesize");
    
    llvm::Value* stack_top = ir_builder->CreateAdd(new_sp, function_framesize);
    llvm::Value* static_link_address = ir_builder->CreateAdd(stack_top, llvm::ConstantInt::get(ir_builder->getInt64Ty(), 8));
    // std::string outfile = "temp.ll";
    // std::error_code EC;
    // llvm::raw_fd_ostream file(outfile, EC);
    // ir_module->print(file, nullptr);
    llvm::Value* static_link_ptr = ir_builder->CreateIntToPtr(static_link_address, ir_builder->getInt64Ty()->getPointerTo());
    ir_builder->CreateStore(llvm_function->getArg(1), static_link_ptr);
    int offset = 16;
    auto param_type_it = formals_typelist->GetList().begin();
    //跳过sp和sl
    param_type_it = param_type_it ++;
    param_type_it = param_type_it ++;
    for(auto param: params_list){
      llvm::Value* param_address = ir_builder->CreateAdd(stack_top, llvm::ConstantInt::get(ir_builder->getInt64Ty(), offset));
      llvm::Value* param_ptr = ir_builder->CreateIntToPtr(param_address, (*param_type_it)->GetLLVMType()->getPointerTo());
      ir_builder->CreateStore(llvm_function->getArg(offset/8), param_ptr);
      offset += 8;
      param_type_it++;
    }

    //这个是frame里存的参数的access，要让后面的函数可以访问，需要加到venv里
    auto current_func_formals = *new_level->frame_->Formals();
    current_func_formals.erase(current_func_formals.begin());
    param_type_it = formals_typelist->GetList().begin();
    //跳过sp和sl
    param_type_it = param_type_it ++;
    param_type_it = param_type_it ++;
    auto param_it = params_list.begin();
    venv->BeginScope();

    for(auto formal: current_func_formals){
      auto param = *param_it;
      // env::VarEntry *var_entry = new env::VarEntry(formal, (*param_type_it));
      tr::Access *translate_access = new tr::Access(new_level, formal);
      venv->Enter(param->name_, new env::VarEntry(translate_access, (*param_type_it)));
      param_it ++;
    }
    func_stack.push(llvm_function);
    frame_info.push_back(std::make_pair(llvm_function->getName().str(), new_level->frame_));


    // std::string outfile = "temp.ll";
    // std::error_code EC;
    // llvm::raw_fd_ostream file(outfile, EC);
    // ir_module->print(file, nullptr);
    // 翻译函数体
    auto body_result = funcDec->body_->Translate(venv, tenv, new_level, errormsg);
    if (!body_result) {
      errormsg->Error(funcDec->pos_, "function body translation failed");
      return;
    }
    auto new_frame_size = new_level->frame_->calculateActualFramesize();
    new_func_framesize_global->setInitializer(llvm::ConstantInt::get(ir_builder->getInt64Ty(),new_level->frame_->calculateActualFramesize()));
    // std::cout << "actual type of body_result " << typeid(*(body_result->ty_)).name() << std::endl;
    // std::cout <<typeid(*(body_result->ty_)).name() << std::endl;
    // std::cout <<typeid(type::VoidTy).name() << std::endl;
    if(typeid(*(body_result->ty_))==typeid(type::VoidTy)){
      ir_builder->CreateRetVoid();
    }else if(funcDec->name_->Name() == "isdigit"){
      //将isdigit的返回值转换为int
      auto int_result = ir_builder->CreateZExt(body_result->val_, ir_builder->getInt32Ty());
      ir_builder->CreateRet(int_result);
    }else{
      ir_builder->CreateRet(body_result->val_);

    }
    ir_builder->SetInsertPoint(current_block);

    func_stack.pop();
    venv->EndScope();
    
  }
}

void VarDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv, tr::Level *level,
                       err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto init_val_ty = init_->Translate(venv, tenv, level, errormsg);

  if (init_val_ty == NULL) {
    errormsg->Error(pos_, "Initilization failed");
    return;
  }
  auto init_val = init_val_ty->val_;
  auto init_ty = init_val_ty->ty_;
  if(typeid(*init_) == typeid(VarExp)){
    auto init_address = init_val_ty->val_;
    auto init_ptr = ir_builder->CreateIntToPtr(init_address, init_ty->GetLLVMType()->getPointerTo());
    init_val = ir_builder->CreateLoad(init_ty->GetLLVMType(),init_ptr);
  }
  auto framesize_global = level->frame_->framesize_global;
  auto access =  level->frame_->AllocLocal(true);
  tr::Access* translate_access = new tr::Access(level,access);
  // llvm::Value* function_frame_size = ir_builder->CreateLoad(ir_builder->getInt64Ty(), framesize_global);
  // llvm::Value* tigermain_sp = level->get_sp();
  // auto var_address= ir_builder->CreateAdd(tigermain_sp, access.);
  // var_address = ir_builder->CreateIntToPtr(var_address, init_ty->GetLLVMType()->getPointerTo());
  auto new_var_address = access->get_inframe_address(level->get_sp(), level->frame_->framesize_global, ir_builder);
  llvm::Value * new_var_ptr = nullptr;
  // std::cout << "actual type of var_e./ntry->ty_ " << typeid(*(init_ty)).name() << std::endl;
  // std::cout << "actual type of var_entry->ty_ " << typeid(ArrayTy).name() << std::endl;

  if(typeid(*init_ty) == typeid(type::ArrayTy)){
    new_var_ptr = ir_builder->CreateIntToPtr(new_var_address, ir_builder->getInt64Ty()->getPointerTo());
  }else{
    new_var_ptr = ir_builder->CreateIntToPtr(new_var_address, init_ty->GetLLVMType()->getPointerTo());
  }
  ir_builder->CreateStore(init_val, new_var_ptr);
  venv->Enter(var_, new env::VarEntry(translate_access, init_ty));
}

type::Ty *NameTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  return tenv->Look(name_);

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

  return new type::RecordTy(ty_fileds);
}

type::Ty *ArrayTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  return new type::ArrayTy(tenv->Look(array_));

}

tr::ValAndTy *SimpleVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                    err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  // std::cout << this->sym_->Name()<<std::endl;
  auto entry = venv->Look(this->sym_);
  assert(entry);
  auto var_entry = dynamic_cast<env::VarEntry *>(entry);
  assert(var_entry);
  auto access = var_entry->access_;
  auto frame_access = access->access_;
  if(var_entry->access_->level_ != level){
    auto current_level = level;
    llvm::Value *var_level_sp = current_level->get_sp();
    while(current_level != var_entry->access_->level_){
      auto static_link = current_level->frame_->Formals()->front();
      auto static_link_address = static_link->get_inframe_address(var_level_sp, current_level->frame_->framesize_global, ir_builder);
      llvm::Value *static_link_ptr = ir_builder->CreateIntToPtr(static_link_address, ir_builder->getInt64Ty()->getPointerTo());
      llvm::Value *static_link_val = ir_builder->CreateLoad(ir_builder->getInt64Ty(), static_link_ptr);
      var_level_sp = static_link_val;
      current_level = current_level->parent_;
    }
    auto val_value = frame_access->get_inframe_address(var_level_sp, current_level->frame_->framesize_global, ir_builder);
    // std::cout << "actual type of var_entry->ty_ " << typeid(*(var_entry->ty_)).name() << std::endl;
    if(typeid(*(var_entry->ty_))==typeid(type::IntTy)){
      return new tr::ValAndTy(val_value, type::IntTy::Instance());
    }else if(typeid(*(var_entry->ty_))==typeid(type::RecordTy)){
      // auto val_ptr = ir_builder->CreateIntToPtr(val_value, var_entry->ty_->GetLLVMType()->getPointerTo());
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }else if(typeid(*(var_entry->ty_))==typeid(type::StringTy)){
      // std::cout << "string type" << std::endl;
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }else if(typeid(*(var_entry->ty_))==typeid(type::ArrayTy)){
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }
    assert(false);
    return nullptr;

  } else{
    auto val_value = frame_access->get_inframe_address(level->get_sp(), level->frame_->framesize_global, ir_builder);
    if(typeid(*(var_entry->ty_))==typeid(type::IntTy)){
      return new tr::ValAndTy(val_value, type::IntTy::Instance());
    }else if(typeid(*(var_entry->ty_))==typeid(type::RecordTy)){
      // auto val_ptr = ir_builder->CreateIntToPtr(val_value, var_entry->ty_->GetLLVMType()->getPointerTo());
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }else if(typeid(*(var_entry->ty_))==typeid(type::StringTy)){
      // std::cout << "string type" << std::endl;
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }else if(typeid(*(var_entry->ty_))==typeid(type::ArrayTy)){
      return new tr::ValAndTy(val_value, var_entry->ty_);
    }
    assert(false);
    return nullptr;
  }

}

tr::ValAndTy *FieldVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  auto var_val_ty = var_->Translate(venv, tenv, level, errormsg);
  auto var_val = var_val_ty->val_; 
  auto var_ty = var_val_ty->ty_;
  type::RecordTy *record_ty = dynamic_cast<type::RecordTy *>(var_ty);
  assert(record_ty);

  llvm::StructType *record_llvm_ty = llvm::cast<llvm::StructType>(record_ty->GetLLVMType()->getPointerElementType());
  llvm::Value *val_ptr = ir_builder->CreateIntToPtr(var_val, record_llvm_ty->getPointerTo()->getPointerTo());
  llvm::Value *record_ptr = ir_builder->CreateLoad(record_llvm_ty->getPointerTo(), val_ptr);

  int field_index = 0;
  for (auto field : record_ty->fields_->GetList()) {
    if (field->name_->Name() == sym_->Name()) {
      llvm::Value *field_ptr = ir_builder->CreateStructGEP(record_llvm_ty, record_ptr, field_index,sym_->Name());
      return new tr::ValAndTy(field_ptr, field->ty_);
    }
    field_index++;
  }
  assert(false);
  //下面可以不要，但是会报warning
  return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
}

tr::ValAndTy *SubscriptVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                      tr::Level *level,
                                      err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto var_val_ty = var_->Translate(venv, tenv, level, errormsg);
  
  llvm::Value *var_address = var_val_ty->val_;
  type::Ty *var_ty = var_val_ty->ty_;

  if (typeid(*var_ty) != typeid(type::ArrayTy)) {
    errormsg->Error(pos_, "not a array type");
    return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }

  auto sub_val_ty = subscript_->Translate(venv, tenv, level, errormsg);
  llvm::Value *sub_val = sub_val_ty->val_;
  type::Ty *sub_ty = sub_val_ty->ty_;
  if(typeid(*subscript_) != typeid(IntExp) && typeid(*subscript_) != typeid(OpExp)){
    auto sub_val_address = sub_val;
    auto sub_val_ptr = ir_builder->CreateIntToPtr(sub_val_address, sub_ty->GetLLVMType()->getPointerTo());
    sub_val = ir_builder->CreateLoad(sub_ty->GetLLVMType(), sub_val_ptr);
  }
  if (typeid(*sub_ty) != typeid(type::IntTy)) {
    assert(false);
  }

  type::ArrayTy *array_ty = dynamic_cast<type::ArrayTy *>(var_ty);
  std::cout << "actual type of var_ty " << typeid(var_).name() << std::endl;
  std::cout << "actual type of subscript " << typeid(subscript_).name() << std::endl;
  auto var_ptr = ir_builder->CreateIntToPtr(var_address, array_ty->GetLLVMType()->getPointerTo());
  auto var_val = ir_builder->CreateLoad(array_ty->GetLLVMType(), var_ptr);
  auto array_ptr = ir_builder->CreateIntToPtr(var_val, array_ty->GetLLVMType(),"array_ptr");

  auto array_element_ptr = ir_builder->CreateGEP(array_ty->GetLLVMType()->getPointerElementType(), array_ptr, sub_val);
  // std::string outfile = "temp.ll";
  // std::error_code EC;
  // llvm::raw_fd_ostream file(outfile, EC);
  // ir_module->print(file, nullptr);
  return new tr::ValAndTy(array_element_ptr, array_ty->ty_);
}

tr::ValAndTy *VarExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  // std::cout << "actual type" << typeid(*var_).name() << std::endl;
  return var_->Translate(venv, tenv, level, errormsg);
}

tr::ValAndTy *NilExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  return new tr::ValAndTy(nullptr, type::NilTy::Instance());
}

tr::ValAndTy *IntExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::Value *val = llvm::ConstantInt::get(ir_builder->getInt32Ty(), val_, true);

  // 2. 返回包含值和类型的 ValAndTy
  return new tr::ValAndTy(val, type::IntTy::Instance());
}

tr::ValAndTy *StringExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::Value *string_val = type::StringTy::CreateGlobalStringStructPtr(str_);
  return new tr::ValAndTy(string_val, type::StringTy::Instance());
}

tr::ValAndTy *CallExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level,
                                 err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  //call a function 
  //1. get the function from venv
  auto current_func = func_stack.top();
  auto func_entry = venv->Look(this->func_);
  if (!func_entry || typeid(*func_entry) != typeid(env::FunEntry)) {
    std::cerr << pos_ << "undefined function" << std::endl;
    assert(false);
  }

  //2. get the corresponding llvm function
  auto function_entry = dynamic_cast<env::FunEntry *>(func_entry);
  assert(function_entry);
  std::vector<llvm::Value *>args;
  //所有类似print printi的内置函数都和main函数在同一个level,也就意味着没有parent
  // if (function_entry->level_!= nullptr) {
  //   tr::Level* right_now_level = level;
  //   //最外层的函数
  //   if(function_entry->level_->parent_ == nullptr){
  //     //push nullptr as static link
  //     args.push_back(nullptr);
  //   }
  // }
  for(Exp *arg :args_->GetList()){

    auto arg_val_ty = arg->Translate(venv, tenv, level, errormsg);
    auto arg_val = arg_val_ty->val_;
    auto arg_ty = arg_val_ty->ty_->GetLLVMType();
    if(typeid(*arg) == typeid(VarExp)){
      auto arg_ptr = ir_builder->CreateIntToPtr(arg_val, arg_ty->getPointerTo());
      llvm::Value * load_val = ir_builder->CreateLoad(arg_ty, arg_ptr);
      args.push_back(load_val);
      continue;
    }
    args.push_back(arg_val_ty->val_);
  }

  int call_outgo_size = args.size() * 8 + 8;
  level->frame_->AllocOutgoSpace(call_outgo_size);
  //接下来添加sp和sl的信息
  //① sp的比较简单,从当前调用的函数里面传进来就可以
  llvm::Value *sp = level->frame_->sp;
  //② sl就稍微复杂一点，要找到静态包含这个func的最近的函数
  llvm::Value *sl = nullptr;
  //  首先，caller只能是callee的父函数、同级或者子函数
  //  假设callee是内嵌函数，和main函数在同一个level，那么sl就是nullptr
  if(function_entry->level_->parent_ == nullptr){
    sl = nullptr;
  }else if(function_entry->level_->parent_ == level){
    //  假设caller恰好就是callee的父函数，那么只需要将caller的sp当作sl传给callee即可
    sl = sp;
  }else{
    // 假设caller是callee的同级或者子函数，就需要通过sl跳转得到正确的值
    auto level_gap = 0;
    auto temp_level = function_entry->level_->parent_;
    while(temp_level != level->parent_){
      temp_level = temp_level->parent_;
      level_gap += 1;
    }
    auto jump_times = level_gap + 1;
    llvm::Value *static_link = nullptr;
    llvm::Value *static_link_address = nullptr;
    llvm::Value *current_sp = sp;
    auto current_level = level;
    for(int i = 0; i < jump_times; i++){
      auto sl_access = current_level->frame_->Formals()->front();
      static_link_address = sl_access->get_inframe_address(current_sp, current_level->frame_->framesize_global, ir_builder);
      llvm::Value *static_link_ptr = ir_builder->CreateIntToPtr(static_link_address, ir_builder->getInt64Ty()->getPointerTo());
      static_link = ir_builder->CreateLoad(ir_builder->getInt64Ty(), static_link_ptr); 
      current_level = current_level->parent_;
      current_sp = static_link;
    }
    sl = static_link;
  }
  if(sl != nullptr){
    //意味着函数不是内嵌函数
    args.insert(args.begin(), sl);
    args.insert(args.begin(), sp);
  }
  auto func_name = this->func_->Name();
  auto func = ir_module->getFunction(func_name);
  llvm::Function * fun = function_entry->func_;
  func_stack.push(fun);
  llvm::Value *call_result = ir_builder->CreateCall(fun, args);
  func_stack.pop();
  return new tr::ValAndTy(call_result, function_entry->result_);
  //3. save space for passing arguments
  //4. transfer the argumens to register

  //5. start a new frame

  //6. call the function
}

tr::ValAndTy *OpExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  // std::cout << "actual type" << typeid(*left_).name() << std::endl;
  // std::cout << "actual type" << typeid(*right_).name() << std::endl;
  bool is_left_need_load = false;
  bool is_right_need_load = false;
  if(typeid(*left_) == typeid(VarExp)){
    is_left_need_load = true;
  }
  if(typeid(*right_) == typeid(VarExp)){
    is_right_need_load = true;
  }
  auto left_val_ty = left_->Translate(venv, tenv, level, errormsg);
  auto right_val_ty = right_->Translate(venv, tenv, level, errormsg);
  llvm::Value* left_val = nullptr;
  llvm::Value* right_val = nullptr;
  auto left_address = left_val_ty->val_;
  auto right_address = right_val_ty->val_;
  auto left_ty = left_val_ty->ty_;
  auto right_ty = right_val_ty->ty_;
  if(is_left_need_load){
    auto left_val_ptr = ir_builder->CreateIntToPtr(left_address, left_ty->GetLLVMType()->getPointerTo());
    left_val = ir_builder->CreateLoad(left_ty->GetLLVMType(), left_val_ptr);  
  }else{
    left_val = left_address;
  }
  if (is_right_need_load) {
    auto right_val_ptr = ir_builder->CreateIntToPtr(right_address, right_ty->GetLLVMType()->getPointerTo());
    right_val = ir_builder->CreateLoad(right_ty->GetLLVMType(), right_val_ptr);
  } else {
    right_val = right_address;
  }
  llvm::Value *result = nullptr;

  if (typeid(*left_ty) == typeid(type::IntTy) &&
      typeid(*right_ty) == typeid(type::IntTy)) {
    switch (oper_) {
    case AND_OP:
      result = ir_builder->CreateAnd(left_val, right_val);
      break;
    case OR_OP:
      result = ir_builder->CreateOr(left_val, right_val);
      break;
    case PLUS_OP:
      result = ir_builder->CreateAdd(left_val, right_val);
      break;
    case MINUS_OP:
      result = ir_builder->CreateSub(left_val, right_val);
      break;
    case TIMES_OP:
      result = ir_builder->CreateMul(left_val, right_val);
      break;
    case DIVIDE_OP:
      result = ir_builder->CreateSDiv(left_val, right_val);
      break;
    case EQ_OP:
      result = ir_builder->CreateICmpEQ(left_val, right_val);
      break;
    case NEQ_OP:
      result = ir_builder->CreateICmpNE(left_val, right_val);
      break;
    case LT_OP:
      result = ir_builder->CreateICmpSLT(left_val, right_val);
      break;
    case LE_OP:
      result = ir_builder->CreateICmpSLE(left_val, right_val);
      break;
    case GT_OP:
      result = ir_builder->CreateICmpSGT(left_val, right_val);
      break;
    case GE_OP:
      result = ir_builder->CreateICmpSGE(left_val, right_val);
      break;
    default:
      assert(false);
    }
    return new tr::ValAndTy(result, type::IntTy::Instance());
  }else if(left_val_ty->ty_->IsSameType(type::StringTy::Instance())){
    if(oper_ == absyn::EQ_OP){
      result = ir_builder->CreateCall(string_equal, {left_val, right_val});
    }else if(oper_ == absyn::NEQ_OP){
      result = ir_builder->CreateNot(ir_builder->CreateCall(string_equal, {left_val, right_val}));
    }else{
      // oper_ not supported
      assert(false);
    }
    return new tr::ValAndTy(result, type::IntTy::Instance());
  }else if(right_val_ty->ty_->IsSameType(type::NilTy::Instance())){
    if (oper_ == absyn::EQ_OP) {
      result = ir_builder->CreateIsNull(left_val);
    } else if (oper_ == absyn::NEQ_OP) {
      result = ir_builder->CreateIsNotNull(left_val);
    }
    return new tr::ValAndTy(result, type::IntTy::Instance());

  }else {
    errormsg->Error(pos_, "Type mismatch");
    assert(false);
    return nullptr;
  }
}

tr::ValAndTy *RecordExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto ty = tenv->Look(typ_);
  type::RecordTy *record_ty = static_cast<type::RecordTy *>(ty);
  llvm::StructType *llvm_record_ty = llvm::cast<llvm::StructType>(record_ty->GetLLVMType()->getPointerElementType());
  llvm::Value* size = llvm::ConstantInt::get(ir_builder->getInt32Ty(),llvm_record_ty->getStructNumElements() * 8);
  llvm::Value* record_ptr = ir_builder->CreateCall(alloc_record,size);

  record_ptr = ir_builder->CreateIntToPtr(record_ptr, llvm_record_ty->getPointerTo());
  auto filed_list = fields_->GetList();
  int index = 0;
  for (auto field : filed_list) {
    auto field_val_ty = field->exp_->Translate(venv, tenv, level, errormsg);
    llvm::Value *field_exp_val = field_val_ty->val_;
    if (typeid(*field->exp_) == typeid(VarExp)) {
      auto field_exp_ptr = ir_builder->CreateIntToPtr(field_exp_val, field_val_ty->ty_->GetLLVMType()->getPointerTo());
      field_exp_val = ir_builder->CreateLoad(field_val_ty->ty_->GetLLVMType(), field_exp_ptr);
    }
    llvm::Value *field_ptr = ir_builder->CreateStructGEP(llvm_record_ty, record_ptr, index, field->name_->Name());
    ir_builder->CreateStore(field_exp_val, field_ptr);
    index++;
  }

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
    // std::cout << "Actual type: " << typeid(*exp).name() << std::endl;
    auto val_ty = exp->Translate(venv, tenv, level, errormsg);
    if (val_ty == nullptr) {
      errormsg->Error(pos_, "Translate failed");
      return nullptr;
    }
    last_val = val_ty->val_;
    last_ty = val_ty->ty_;
    if(typeid(*exp)==typeid(VarExp)){
      std::cout << "这俩相等" <<std::endl;
      auto last_val_address = val_ty->val_;
      auto last_val_ptr = ir_builder->CreateIntToPtr(last_val_address, last_ty->GetLLVMType()->getPointerTo());
      last_val = ir_builder->CreateLoad(last_ty->GetLLVMType(),last_val_ptr);
    }
  }
  return new tr::ValAndTy(last_val, last_ty);
}

tr::ValAndTy *AssignExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  // std::cout <<"entered assign exp"<<std::endl;
  auto exp_val_ty = exp_->Translate(venv, tenv, level, errormsg);
  assert(exp_val_ty && exp_val_ty->val_);
  auto exp_val = exp_val_ty->val_;
  auto exp_ty = exp_val_ty->ty_;
  if(typeid(*exp_) == typeid(VarExp)){
    auto exp_address = exp_val_ty->val_ ;
    auto exp_ptr = ir_builder->CreateIntToPtr(exp_address, exp_ty->GetLLVMType()->getPointerTo());
    exp_val = ir_builder->CreateLoad(exp_ty->GetLLVMType(),exp_ptr);
  }

  auto var_address_ty = var_->Translate(venv, tenv, level, errormsg);
  assert(var_address_ty && var_address_ty->val_);
  auto var_address = var_address_ty->val_;
  auto var_ty = var_address_ty->ty_;
  auto var_ptr = ir_builder->CreateIntToPtr(var_address, var_ty->GetLLVMType()->getPointerTo());
  ir_builder->CreateStore(exp_val, var_ptr);
  // std::cout <<"actual type of var is:" <<typeid(*var_ty).name()<<std::endl;
  // std::cout <<"actual type of exp is:" <<typeid(*exp_ty).name()<<std::endl;
  return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
}

tr::ValAndTy *IfExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto test_val_ty = test_->Translate(venv, tenv, level, errormsg);
  if (typeid(*test_) == typeid(VarExp) && typeid(*test_val_ty->ty_) != typeid(type::RecordTy)) {
    auto test_val_address = test_val_ty->val_;
    auto test_val_ptr = ir_builder->CreateIntToPtr(test_val_address, test_val_ty->ty_->GetLLVMType()->getPointerTo());
    auto test_val = ir_builder->CreateLoad(test_val_ty->ty_->GetLLVMType(),test_val_ptr);
    test_val_ty->val_ = test_val;
  }else if (typeid(*test_) == typeid(VarExp) && typeid(*test_val_ty->ty_) == typeid(type::RecordTy)) {
    if (typeid(*static_cast<VarExp *>(test_)->var_) == typeid(SimpleVar)) {
      llvm::StructType *llvm_record_ty = llvm::cast<llvm::StructType>(test_val_ty->ty_->GetLLVMType()->getPointerElementType());
      auto test_val_address = test_val_ty->val_;
      auto test_val_ptr = ir_builder->CreateIntToPtr(test_val_address, ir_builder->getInt64Ty()->getPointerTo());
      llvm::Value* test_val = ir_builder->CreateLoad(ir_builder->getInt64Ty(),test_val_ptr);
      auto struc_ptr = ir_builder->CreateIntToPtr(test_val, llvm_record_ty->getPointerTo());
      test_val_ty->val_ = struc_ptr;
    }
  }
  if (!test_val_ty->val_->getType()->isIntegerTy(1)) {
    test_val_ty->val_ = ir_builder->CreateICmpNE(test_val_ty->val_,llvm::ConstantInt::get(llvm::Type::getInt32Ty(ir_builder->getContext()),0));
  }
  int loop_depth = loop_stack.size();
  llvm::Function *func = ir_builder->GetInsertBlock()->getParent();

  llvm::BasicBlock *then_bb  = llvm::BasicBlock::Create(ir_module->getContext(), "if_then", func);
  llvm::BasicBlock *else_bb  = llvm::BasicBlock::Create(ir_module->getContext(), "if_else");
  llvm::BasicBlock *merge_bb = llvm::BasicBlock::Create(ir_module->getContext(), "if_next");
  if (!elsee_) {
    ir_builder->CreateCondBr(test_val_ty->val_, then_bb, merge_bb);
  } else {
    ir_builder->CreateCondBr(test_val_ty->val_, then_bb, else_bb);
  }
  ir_builder->SetInsertPoint(then_bb);
  func->getBasicBlockList().push_back(then_bb);
  auto then_val_ty = then_->Translate(venv, tenv, level, errormsg);
  // errormsg->Error(757, "then wrong");
  if (then_val_ty->ty_&&typeid(*then_) == typeid(VarExp) && typeid(*then_val_ty->ty_) != typeid(type::RecordTy)) {
    auto then_val_address = then_val_ty->val_;
    auto then_val_ptr = ir_builder->CreateIntToPtr(then_val_address, then_val_ty->ty_->GetLLVMType()->getPointerTo());
    auto then_val = ir_builder->CreateLoad(then_val_ty->ty_->GetLLVMType(),then_val_ptr);
    then_val_ty->val_ = then_val;
  }else if (then_val_ty->ty_&&typeid(*then_) == typeid(VarExp) && typeid(*then_val_ty->ty_) == typeid(type::RecordTy)) {
    if (typeid(*static_cast<VarExp *>(then_)->var_) == typeid(SimpleVar)) {
      llvm::StructType *llvm_record_ty = llvm::cast<llvm::StructType>(then_val_ty->ty_->GetLLVMType()->getPointerElementType());
      auto then_val_address = then_val_ty->val_;
      auto then_val_ptr = ir_builder->CreateIntToPtr(then_val_address, ir_builder->getInt64Ty()->getPointerTo());
      llvm::Value* then_val = ir_builder->CreateLoad(ir_builder->getInt64Ty(),then_val_ptr);
      auto struc_ptr = ir_builder->CreateIntToPtr(then_val, llvm_record_ty->getPointerTo());
      then_val_ty->val_ = struc_ptr;
    }
  }
  if (loop_depth == loop_stack.size()) {
    //可以跳转到 if_next
    ir_builder->CreateBr(merge_bb);
  }
  if (!elsee_) {
    func->getBasicBlockList().push_back(merge_bb);
    ir_builder->SetInsertPoint(merge_bb);
    return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }
  func->getBasicBlockList().push_back(else_bb);
  ir_builder->SetInsertPoint(else_bb);
  tr::ValAndTy *else_val_ty = nullptr;
  if (elsee_) {
    else_val_ty = elsee_->Translate(venv, tenv, level, errormsg);
  }else{
    else_val_ty = new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }
  // std::cout << "actual type of el:" << typeid(*(else_val_ty->ty_)).name() << std::endl;
  if (else_val_ty->ty_ && typeid(*else_val_ty->ty_) != typeid(type::RecordTy) && typeid(*elsee_) == typeid(VarExp)) {
    auto else_val_address = else_val_ty->val_;
    auto else_val_ptr = ir_builder->CreateIntToPtr(else_val_address, else_val_ty->ty_->GetLLVMType()->getPointerTo());
    auto else_val = ir_builder->CreateLoad(else_val_ty->ty_->GetLLVMType(),else_val_ptr);
    else_val_ty->val_ = else_val;
  }
  else if (typeid(*elsee_) == typeid(VarExp) && typeid(*else_val_ty->ty_) == typeid(type::RecordTy)) {
    if (typeid(*static_cast<VarExp *>(elsee_)->var_) == typeid(SimpleVar)) {
      llvm::StructType *llvm_record_ty = llvm::cast<llvm::StructType>(else_val_ty->ty_->GetLLVMType()->getPointerElementType());
      auto else_val_address = else_val_ty->val_;
      auto else_val_ptr = ir_builder->CreateIntToPtr(else_val_address, ir_builder->getInt64Ty()->getPointerTo());
      llvm::Value* else_val = ir_builder->CreateLoad(ir_builder->getInt64Ty(),else_val_ptr);
      auto struc_ptr = ir_builder->CreateIntToPtr(else_val, llvm_record_ty->getPointerTo());
      else_val_ty->val_ = struc_ptr;
    }
  }
  ir_builder->CreateBr(merge_bb);
  else_bb = ir_builder->GetInsertBlock();

  func->getBasicBlockList().push_back(merge_bb);
  ir_builder->SetInsertPoint(merge_bb);
  //调用print的时候会出现这个情况
  if(then_val_ty->val_ != nullptr && then_val_ty->ty_ == nullptr){
    return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }
  if (then_val_ty->ty_->IsSameType(type::VoidTy::Instance())) {
    return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
  }
  llvm::PHINode *phi_node = ir_builder->CreatePHI(then_val_ty->val_->getType(), 2, "iftmp");
  std::cout << "then type" <<then_val_ty->val_->getType() <<std::endl;
  phi_node->addIncoming(then_val_ty->val_, then_bb);
  if (!else_val_ty->val_) {
    assert(then_val_ty->val_->getType()->isPointerTy());
    llvm::PointerType *struct_type = llvm::cast<llvm::PointerType>(then_val_ty->val_->getType());
    else_val_ty->val_ = llvm::ConstantPointerNull::get(struct_type);
  }
  std::cout << "else type" <<else_val_ty->val_->getType()<<std::endl;
  phi_node->addIncoming(else_val_ty->val_, else_bb);
  // std::cout << "actual type of then_val:" << typeid(*then_val_ty->ty_).name() << std::endl;
  // std::cout << "actual type of else_val:" << typeid(*else_val_ty->ty_).name() << std::endl;
  if (phi_node->getType()->isPointerTy() &&  typeid(*then_val_ty->ty_) != typeid(type::RecordTy) && !then_val_ty->ty_->IsSameType(type::StringTy::Instance())) {
    return new tr::ValAndTy(ir_builder->CreateLoad(phi_node->getType()->getPointerElementType(),phi_node),then_val_ty->ty_);
  }
  return new tr::ValAndTy(phi_node, then_val_ty->ty_);
}

tr::ValAndTy *WhileExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::Function *current_func = func_stack.top();
  // loop_stack.push(current_func);
  // loop_stack.push()
  llvm::BasicBlock *test_label = llvm::BasicBlock::Create(ir_builder->getContext(), "while_test", current_func);
  llvm::BasicBlock *body_label = llvm::BasicBlock::Create(ir_builder->getContext(), "while_body", current_func);
  llvm::BasicBlock *done_label = llvm::BasicBlock::Create(ir_builder->getContext(), "while_next", current_func);
  loop_stack.push(done_label);
  ir_builder->CreateBr(test_label);
  ir_builder->SetInsertPoint(test_label);
  auto test_val_ty = test_->Translate(venv, tenv, level, errormsg);
  assert(test_val_ty && test_val_ty->ty_);

  llvm::Value* test_val = test_val_ty->val_;
  llvm::Value* condition = ir_builder->CreateICmpNE(test_val, llvm::ConstantInt::get(test_val->getType(), 0), "to_bool");
  auto test_ty = test_val_ty->ty_;
  // std::cout << "test_ty: " << typeid(*test_ty).name() << std::endl;
  ir_builder->CreateCondBr(condition,body_label,done_label);
  ir_builder->SetInsertPoint(body_label);
  auto body_val_ty = body_->Translate(venv, tenv, level, errormsg);
  assert(body_val_ty);

  ir_builder->CreateBr(test_label);

  ir_builder->SetInsertPoint(done_label);

  loop_stack.pop();
  return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
}

tr::ValAndTy *ForExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::Function *func = ir_builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *incre_bb = llvm::BasicBlock::Create(ir_builder->getContext(), "for_incre", func);
  llvm::BasicBlock *body_bb  = llvm::BasicBlock::Create(ir_builder->getContext(), "for_body");
  llvm::BasicBlock *exit_bb  = llvm::BasicBlock::Create(ir_builder->getContext(), "for_exit");

  loop_stack.push(exit_bb);

  tr::ValAndTy *lo_val_ty = lo_->Translate(venv, tenv, level, errormsg);
  tr::ValAndTy *hi_val_ty = hi_->Translate(venv, tenv, level, errormsg);
  // std::cout << "actual type of lo:" << typeid(lo_).name() << std::endl;
  // std::cout << "actual type of hi:" << typeid(hi_).name() << std::endl;

  if (typeid(*lo_) == typeid(VarExp)) {
    auto lo_var_address = lo_val_ty->val_;
    auto lo_var_ptr = ir_builder->CreateIntToPtr(lo_var_address, lo_val_ty->ty_->GetLLVMType()->getPointerTo());
    lo_val_ty->val_ = ir_builder->CreateLoad(lo_val_ty->ty_->GetLLVMType(), lo_var_ptr);
  }
  llvm::Value *hi_var_val = hi_val_ty->val_;
  if (typeid(*hi_) == typeid(VarExp)) {
    auto hi_var_address = hi_val_ty->val_;
    auto hi_var_ptr = ir_builder->CreateIntToPtr(hi_var_address, hi_val_ty->ty_->GetLLVMType()->getPointerTo());
    hi_var_val = ir_builder->CreateLoad(hi_val_ty->ty_->GetLLVMType(), hi_var_ptr);
  }
  if (!lo_val_ty->ty_->IsSameType(type::IntTy::Instance()) || !hi_val_ty->ty_->IsSameType(type::IntTy::Instance())) {
    assert(false);
  }

  venv->BeginScope();
  frame::Access *access = level->frame_->AllocLocal(lo_val_ty->val_);
  venv->Enter(var_, new env::VarEntry(new tr::Access(level, access),type::IntTy::Instance()));
  auto cond_var_address =  access->get_inframe_address(level->frame_->sp, level->frame_->framesize_global, ir_builder);
  llvm::Value *cond_var_ptr = ir_builder->CreateIntToPtr(
      cond_var_address, lo_val_ty->val_->getType()->getPointerTo());
  ir_builder->CreateStore(lo_val_ty->val_, cond_var_ptr);
  ir_builder->CreateBr(body_bb);
  ir_builder->SetInsertPoint(incre_bb);
  llvm::Value *var_val = ir_builder->CreateLoad(lo_val_ty->ty_->GetLLVMType(), cond_var_ptr);
  var_val = ir_builder->CreateAdd(var_val, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ir_builder->getContext()), 1));
  // std::string outfile = "temp.ll";
  // std::error_code EC;
  // llvm::raw_fd_ostream file(outfile, EC);
  // ir_module->print(file, nullptr);
  ir_builder->CreateStore(var_val, cond_var_ptr);
  llvm::Value *cond = ir_builder->CreateICmpSLE(var_val, hi_var_val);
  ir_builder->CreateCondBr(cond, body_bb, exit_bb);
  func->getBasicBlockList().push_back(body_bb);
  ir_builder->SetInsertPoint(body_bb);
  body_->Translate(venv, tenv, level, errormsg);
  ir_builder->CreateBr(incre_bb);
  func->getBasicBlockList().push_back(exit_bb);
  ir_builder->SetInsertPoint(exit_bb);
  loop_stack.pop();
  venv->EndScope();

  return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
}

tr::ValAndTy *BreakExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  llvm::BasicBlock * the_going_block = loop_stack.top();
  ir_builder->CreateBr(the_going_block);
  loop_stack.pop();
  return new tr::ValAndTy(nullptr,type::VoidTy::Instance());
}

tr::ValAndTy *LetExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level,
                                err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  auto decs_list = this->decs_->GetList();
  venv->BeginScope();
  tenv->BeginScope();
  for (auto dec : decs_list) {
    dec->Translate(venv, tenv, level, errormsg);
  }
  // std::cout << "Actual type: " << typeid(*body_).name() << std::endl;
  auto res = body_->Translate(venv, tenv, level, errormsg);
  tenv->EndScope();
  venv->EndScope();
  return res;
}

tr::ValAndTy *ArrayExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level,
                                  err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  type::Ty *ty = tenv->Look(typ_);
  assert(ty);
  if (typeid(*ty) != typeid(type::ArrayTy)) {
    assert(false);
  }

  auto size_val_ty = size_->Translate(venv, tenv, level, errormsg);
  auto init_val_ty = init_->Translate(venv, tenv, level, errormsg);

  llvm::Value *array_size = size_val_ty->val_;
  llvm::Value *array_init = init_val_ty->val_;
  if(typeid(*(size_)) == typeid(VarExp)){
    auto arraysize_address = size_val_ty->val_;
    auto arraysize_ptr = ir_builder->CreateIntToPtr(arraysize_address, size_val_ty->ty_->GetLLVMType()->getPointerTo());
    array_size = ir_builder->CreateLoad(size_val_ty->ty_->GetLLVMType(),arraysize_ptr);
  }
  if(typeid(*(init_)) == typeid(VarExp)){
    auto init_address = init_val_ty->val_;
    auto init_ptr = ir_builder->CreateIntToPtr(init_address, size_val_ty->ty_->GetLLVMType()->getPointerTo());
    array_init = ir_builder->CreateLoad(size_val_ty->ty_->GetLLVMType(),init_ptr);
  }
  array_init = ir_builder->CreateSExt(array_init, llvm::Type::getInt64Ty(ir_module->getContext()));
  std::vector<llvm::Value *> array_args;
  assert(array_init->getType()->isIntegerTy(64));
  array_args.push_back(array_size);
  array_args.push_back(array_init);
  llvm::Value *array_ptr = ir_builder->CreateCall(init_array, array_args,"array_ptr");
  // std::string outfile = "temp.ll";
  // std::error_code EC;
  // llvm::raw_fd_ostream file(outfile, EC);
  // ir_module->print(file, nullptr);

  return new tr::ValAndTy(array_ptr, ty);
}

tr::ValAndTy *VoidExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level,
                                 err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab5-part1 code here */
  return new tr::ValAndTy(nullptr, type::VoidTy::Instance());
}

} // namespace absyn