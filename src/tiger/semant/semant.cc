#include "tiger/absyn/absyn.h"
#include "tiger/env/env.h"
#include "tiger/semant/semant.h"
#include "tiger/semant/types.h"
#include <cassert>
#include <iostream>

namespace absyn {

void AbsynTree::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                           err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  root_->SemAnalyze(venv, tenv, 0, errormsg);
}

type::Ty *SimpleVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  sym::Symbol *symbol_to_search = this->sym_;
  env::EnvEntry* value_entry = venv->Look(symbol_to_search);
  if(value_entry != nullptr){
    return (static_cast<env::VarEntry*>(value_entry))->ty_->ActualTy();
  }else{
    errormsg->Error(pos_, "undefined variable %s", symbol_to_search->Name().data());
    return type::VoidTy::Instance();
  }
}

type::Ty *FieldVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* record_ty = this->var_->SemAnalyze(venv,tenv,labelcount,errormsg)->ActualTy();

  if(typeid(*record_ty) != typeid(type::RecordTy)){
    errormsg->Error(pos_,"not a record type");
    return type::VoidTy::Instance();
  }
  auto actual_record_ty = dynamic_cast<type::RecordTy *>(record_ty);
  assert(actual_record_ty);
  auto record_field_list = actual_record_ty->fields_->GetList();
  for(auto single_record:record_field_list){
    if(single_record->name_ == sym_){
      return single_record->ty_->ActualTy();
    }
  }
  errormsg->Error(pos_,"field %s doesn't exist",sym_->Name().data());
  return type::VoidTy::Instance();
}

type::Ty *SubscriptVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   int labelcount,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* array_ty = this->var_->SemAnalyze(venv,tenv,labelcount,errormsg)->ActualTy();
  if(typeid(*array_ty) != typeid(type::ArrayTy)){
    errormsg->Error(pos_,"array type required");
    return type::VoidTy::Instance();
  }
  type::Ty* subscript_ty = this->subscript_->SemAnalyze(venv,tenv,labelcount,errormsg)->ActualTy();
  if(!subscript_ty->IsSameType(type::IntTy::Instance())){
    errormsg->Error(pos_,"subscript must be an int"); 
    return type::VoidTy::Instance();
  }
  auto actual_array_ty = dynamic_cast<type::ArrayTy *>(array_ty);
  assert(actual_array_ty);
  return actual_array_ty->ty_->ActualTy();
}

type::Ty *VarExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* var_ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();

  return var_ty;
}

type::Ty *NilExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::NilTy::Instance();
}

type::Ty *IntExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::IntTy::Instance();
}

type::Ty *StringExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::StringTy::Instance();
}

type::Ty *CallExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  env::EnvEntry* func_entry = venv->Look(func_);
  if(func_entry == nullptr){
    errormsg->Error(pos_, "undefined function %s", func_->Name().data());
    return type::VoidTy::Instance();
  }
  if(typeid(*func_entry) != typeid(env::FunEntry)){
    errormsg->Error(pos_, "%s is not a function", func_->Name().data());
    return type::VoidTy::Instance();
  }
  auto actual_func_entry = dynamic_cast<env::FunEntry *>(func_entry);
  assert(actual_func_entry);
  auto formal_list = actual_func_entry->formals_->GetList();
  auto args_list = args_->GetList();

  if(formal_list.size() < args_list.size()){
    errormsg->Error(pos_,"too many params in function %s",func_->Name().data());
    return (reinterpret_cast<env::FunEntry *> (func_entry))->result_;
  }
  if(formal_list.size() > args_list.size()){
    errormsg->Error(pos_,"para type mismatch");
    return (reinterpret_cast<env::FunEntry *> (func_entry))->result_;
  }

  auto formal_begin = formal_list.begin();
  auto args_begin = args_list.begin();
  while(args_begin!=args_list.end()){
    type::Ty *arg_type = (*args_begin)->SemAnalyze(venv,tenv,labelcount,errormsg);
    
    if(!arg_type->IsSameType(*formal_begin)){
      errormsg->Error(pos_,"para type mismatch");
    }
    args_begin++;
    formal_begin++;
  }
  return actual_func_entry->result_;
}

type::Ty *OpExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* left_ty = left_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty* right_ty = right_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(oper_ == absyn::PLUS_OP || oper_ == absyn::MINUS_OP || oper_ == absyn::TIMES_OP || oper_ == absyn::DIVIDE_OP){
    // + - * /
    // left & right must be int
    if(typeid(*left_ty)!=typeid(type::IntTy)||typeid(*right_ty)!=typeid(type::IntTy)){
      errormsg->Error(pos_, "integer required!");
    }
    return type::IntTy::Instance();
  }
  if(!left_ty->IsSameType(right_ty)){
    errormsg->Error(pos_, "same type required");
    return type::IntTy::Instance();
  }
  
  return type::IntTy::Instance();
}

type::Ty *RecordExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  type::Ty* record_ty = tenv->Look(typ_);
  if(record_ty == nullptr){
    errormsg->Error(pos_, "undefined type %s", typ_->Name().data());
    return type::VoidTy::Instance();
  }
  auto actual_record_ty = dynamic_cast<type::RecordTy *>(record_ty->ActualTy());
  if(!actual_record_ty){
    errormsg->Error(pos_, "%s is not a record", typ_->Name().data());
    return type::VoidTy::Instance();
  }

  auto record_field_list = actual_record_ty->fields_->GetList();
  auto exp_field_list = fields_->GetList();

  if(record_field_list.size() != exp_field_list.size()){
    errormsg->Error(pos_, "not providing correct fields num");
    return record_ty->ActualTy();
  }
  
  auto record_field_list_begin = record_field_list.begin();
  auto exp_field_list_begin = exp_field_list.begin();
  while(record_field_list_begin != record_field_list.end()){
    if((*record_field_list_begin)->name_ != (*exp_field_list_begin)->name_){
      errormsg->Error(pos_, "providing wrong field name");
      return record_ty->ActualTy();
    }
    auto exp_field_ty = (*exp_field_list_begin)->exp_->SemAnalyze(venv, tenv, labelcount, errormsg);
    auto record_field_ty = (*record_field_list_begin)->ty_;
    if(!exp_field_ty->IsSameType(record_field_ty)){
      errormsg->Error(pos_, "providing wrong field type");
      return record_ty->ActualTy();
    }
    record_field_list_begin++;
    exp_field_list_begin++;
  }
  return record_ty->ActualTy();
}

type::Ty *SeqExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto exp_list = seq_->GetList();
  auto exp_list_begin = exp_list.begin();
  type::Ty* result_ty = nullptr;

  while(exp_list_begin != exp_list.end()){
    result_ty = (*exp_list_begin)->SemAnalyze(venv, tenv, labelcount, errormsg);
    exp_list_begin ++;
  }
  return result_ty;
}

type::Ty *AssignExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto var_ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg);
  
  if(typeid(*var_) == typeid(absyn::SimpleVar)){
    auto simple_var_  = dynamic_cast<SimpleVar *>(var_);
    assert(simple_var_);
    auto var_entry = venv->Look(simple_var_->sym_);
    if(typeid(*var_entry) == typeid(env::VarEntry)){
      auto actual_var_entry = dynamic_cast<env::VarEntry *>(var_entry);
      assert(actual_var_entry);
      if(actual_var_entry->readonly_){
        errormsg->Error(pos_, "loop variable can't be assigned");
      }
    }
  }
  auto exp_ty = exp_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!var_ty->IsSameType(exp_ty)){
    errormsg->Error(pos_, "unmatched assign exp");
  }

  return type::VoidTy::Instance();
}

type::Ty *IfExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto test_ty = test_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if(typeid(*test_ty)!=typeid(type::IntTy)){
    errormsg->Error(pos_, "the test of ifexp must be an int");
  }
  auto then_ty = then_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(elsee_ != nullptr){
    auto else_ty = elsee_->SemAnalyze(venv, tenv, labelcount, errormsg);
    //lab5 nil case
    if(!else_ty->IsSameType(then_ty)){
      errormsg->Error(pos_, "then exp and else exp type mismatch");
    }
  }else{
    if(typeid(*then_ty) != typeid(type::VoidTy)){
      errormsg->Error(pos_, "if-then exp's body must produce no value");
    }
  }
  return then_ty->ActualTy();
}

type::Ty *WhileExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto test_ty = test_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(typeid(*test_ty) != typeid(type::IntTy)){
    errormsg->Error(pos_, "the condition must be an int");
  }
  auto body_ty = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg)->ActualTy();
  if(typeid(*body_ty) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "while body must produce no value");
  }
  return type::VoidTy::Instance();
}

type::Ty *ForExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  auto lo_ty = lo_->SemAnalyze(venv, tenv, labelcount, errormsg);
  auto hi_ty = hi_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!(typeid(*lo_ty) == typeid(type::IntTy) && typeid(*hi_ty) == typeid(type::IntTy))){
    errormsg->Error(pos_, "for exp's range type is not integer");
  }
  venv->Enter(var_, new env::VarEntry(type::IntTy::Instance(), true));
  //label + 1, enter the loop
  auto body_ty = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg);
  if(typeid(*body_ty) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "body must be a void exp");
  }
  venv->EndScope();
  return type::VoidTy::Instance();
}

type::Ty *BreakExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  if(labelcount == 0){
    errormsg->Error(pos_, "break is not inside any loop");
  }
  return type::VoidTy::Instance();
}

type::Ty *LetExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  tenv->BeginScope();
  for(auto dec : decs_->GetList()){
    dec->SemAnalyze(venv, tenv, labelcount, errormsg);
  }
  type::Ty * result = nullptr;
  if(!body_){
    result = type::VoidTy::Instance();
  }else{
    result = body_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  }
  tenv->EndScope();
  venv->EndScope();
  return result;
}

type::Ty *ArrayExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto arr_ty = tenv->Look(typ_)->ActualTy();
  if(typeid(*arr_ty) != typeid(type::ArrayTy)){
    errormsg->Error(pos_, "%s is not an array", typ_->Name().data());
  }
  if(arr_ty == nullptr){
    errormsg->Error(pos_, "%s is not defined", typ_->Name().data());
  }
  auto actual_arr_ty = dynamic_cast<type::ArrayTy *>(arr_ty);
  assert(actual_arr_ty);
  auto arr_element_ty = actual_arr_ty->ty_;
  auto size_ty = size_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  auto init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();

  if(typeid(*size_ty) != typeid(type::IntTy)){
    errormsg->Error(pos_, "size must be a int");
  }
  //lab5 nil case
  if(!arr_element_ty->IsSameType(init_ty)){
    errormsg->Error(pos_, "type mismatch");
  }

  return arr_ty;
}

type::Ty *VoidExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto func_decs = functions_->GetList();
  auto func_dec_it = func_decs.begin();
  // part 1 insert all function into venv
  while(func_dec_it != func_decs.end()){
    if(venv->Look((*func_dec_it)->name_) != nullptr){
      errormsg->Error(pos_, "two functions have the same name");
      return;
    }
    auto formals = (*func_dec_it)->params_->MakeFormalTyList(tenv, errormsg);
    type::Ty* result_ty = type::VoidTy::Instance();
    if((*func_dec_it)->result_ != nullptr){
      result_ty = tenv->Look((*func_dec_it)->result_)->ActualTy();
      assert(result_ty);
    }
    venv->Enter((*func_dec_it)->name_, new env::FunEntry(formals, result_ty));
    func_dec_it ++;
  }
  func_dec_it = func_decs.begin();
  while(func_dec_it != func_decs.end()){
    venv->BeginScope();
    auto entry = venv->Look((*func_dec_it)->name_);
    auto func_entry = dynamic_cast<env::FunEntry *>(entry);
    assert(func_entry);
    auto formal_list = func_entry->formals_->GetList();
    auto param_list = (*func_dec_it)->params_->GetList();
    auto formal_it = formal_list.begin();
    auto param_it = param_list.begin();
    while(formal_it != formal_list.end()){
      venv->Enter((*param_it)->name_, new env::VarEntry(*formal_it, false));
      formal_it ++;
      param_it ++;
    }
    auto body_ty = (*func_dec_it)->body_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
    if(!body_ty->IsSameType(func_entry->result_)){
      errormsg->Error(pos_, "procedure returns value");
    }
    venv->EndScope();
    func_dec_it ++;
  }

}

void VarDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                        err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typ_ == nullptr){
    if(typeid(*init_ty) == typeid(type::NilTy)){
      errormsg->Error(pos_, "init should not be nil without type specified");
      return;
    }
    venv->Enter(var_, new env::VarEntry(init_ty));
  }else{
    auto type_ty = tenv->Look(typ_);
    assert(type_ty);
    if(!type_ty->IsSameType(init_ty)){
      errormsg->Error(pos_, "type mismatch");
      return;
    }
    venv->Enter(var_, new env::VarEntry(type_ty));
  }
}

void TypeDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                         err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  // Step 1: Iterate through all type declarations and check for duplicate names
  for (auto type_dec : types_->GetList()) {
    if (tenv->Look(type_dec->name_) != nullptr) {
      errormsg->Error(pos_, "two types have the same name");
      return;
    }
    tenv->Enter(type_dec->name_, new type::NameTy(type_dec->name_, nullptr));
  }

  // Step 2: Resolve each NameAndTy's type
  for (auto type_dec : types_->GetList()) {
    auto *name_ty = static_cast<type::NameTy *>(tenv->Look(type_dec->name_));
    name_ty->ty_ = type_dec->ty_->SemAnalyze(tenv, errormsg);
  }
  // Step 3: Check for type cycles
  for (auto type_dec : types_->GetList()) {
    auto *name_ty = static_cast<type::NameTy *>(tenv->Look(type_dec->name_));
    auto *current_ty = name_ty;
    while (true) {
      auto *next_ty = current_ty->ty_;
      if (typeid(*next_ty) != typeid(type::NameTy)) {
        break;
      }
      auto *next_name_ty = static_cast<type::NameTy *>(next_ty);
      if (next_name_ty->sym_ == name_ty->sym_) {
        errormsg->Error(pos_, "illegal type cycle");
        return;
      }
      current_ty = next_name_ty;
    }
  }
}

type::Ty *NameTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  auto name_ty = tenv->Look(name_);
  if(name_ty == nullptr){
    errormsg->Error(pos_, "undefined type %s", name_->Name().data());
    return type::VoidTy::Instance();
  }
  return new type::NameTy(name_, name_ty);
}

type::Ty *RecordTy::SemAnalyze(env::TEnvPtr tenv,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return new type::RecordTy(record_->MakeFieldList(tenv, errormsg));
}

type::Ty *ArrayTy::SemAnalyze(env::TEnvPtr tenv,
                              err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* arr_ty = tenv->Look(array_);
  if(arr_ty == nullptr){
    errormsg->Error(pos_, "undefined type %s", array_->Name().data());
    return type::VoidTy::Instance();
  }
  return new type::ArrayTy(arr_ty->ActualTy());
}

} // namespace absyn

namespace sem {

void ProgSem::SemAnalyze() {
  FillBaseVEnv();
  FillBaseTEnv();
  absyn_tree_->SemAnalyze(venv_.get(), tenv_.get(), errormsg_.get());
}
} // namespace sem