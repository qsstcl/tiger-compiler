#include "tiger/absyn/absyn.h"
#include "tiger/semant/semant.h"
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
  std::list<type::Field *> record_field_list = (reinterpret_cast<type::RecordTy*> (record_ty)) -> fields_->GetList();
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
  return (reinterpret_cast<type::ArrayTy *>(array_ty))->ty_->ActualTy();
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
  std::list<type::Ty*> formal_list = (static_cast<env::FunEntry*>(func_entry))->formals_->GetList();
  std::list<absyn::Exp*> args_list = args_->GetList();

  if(formal_list.size() > args_list.size()){
    errormsg->Error(pos_,"para type mismatch");
    return (reinterpret_cast<env::FunEntry *> (func_entry))->result_;
  }
  if(formal_list.size() < args_list.size()){
    errormsg->Error(pos_,"too many params in function %s",func_->Name().data());
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
  return (reinterpret_cast<env::FunEntry *> (func_entry))->result_;
}

type::Ty *OpExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */

  //AcutalTy函数仅仅对于NameTy去找到这个类型别名的真正类型
  //获取OP表达式二元各自的类型
  type::Ty * left_ty = left_->SemAnalyze(venv,tenv,labelcount,errormsg);
  type::Ty * right_ty = right_->SemAnalyze(venv,tenv,labelcount,errormsg);
  //如果是加减乘除的操作，左右都必须为int类型
  if(oper_==absyn::PLUS_OP||oper_==absyn::MINUS_OP||oper_==absyn::TIMES_OP||oper_==absyn::DIVIDE_OP){
    if(!( left_ty->IsSameType(type::IntTy::Instance()) && right_ty->IsSameType(type::IntTy::Instance()))){
      errormsg->Error(pos_,"integer required");
      return type::VoidTy::Instance();
    }
    return type::IntTy::Instance();
  }

  //其他操作则左右两个数据必须是相同的类型
  if(!left_ty->IsSameType(right_ty)){
    errormsg->Error(pos_,"same type required");
    return type::VoidTy::Instance();
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
  
  if(typeid(*(record_ty->ActualTy())) != typeid(type::RecordTy)){
    errormsg->Error(pos_, "%s is not a record", typ_->Name().data());
    return record_ty->ActualTy();
  }
  
  std::list<type::Field *> record_field_list = (reinterpret_cast<type::RecordTy *>(record_ty->ActualTy()))->fields_->GetList();
  std::list<absyn::EField *> exp_field_list = fields_->GetList();

  if(record_field_list.size() != exp_field_list.size()){
    errormsg->Error(pos_, "not providing correct fields num");
    return record_ty->ActualTy();
  }
  
  std::list<type::Field *>::iterator record_field_list_begin = record_field_list.begin();
  std::list<absyn::EField *>::iterator exp_field_list_begin = exp_field_list.begin();
  while(record_field_list_begin != record_field_list.end()){
    if((*record_field_list_begin)->name_ != (*exp_field_list_begin)->name_){
      errormsg->Error(pos_, "providing wrong field name");
      return record_ty->ActualTy();
    }
    type::Ty* efield_ty = (*exp_field_list_begin)->exp_->SemAnalyze(venv, tenv, labelcount, errormsg);
    type::Ty* field_ty = (*record_field_list_begin)->ty_;
    if(!efield_ty->IsSameType(field_ty)){
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
  std::list<absyn::Exp *> exp_list = seq_->GetList();
  std::list<absyn::Exp *>::iterator exp_list_begin = exp_list.begin();

  type::Ty* result_ty;

  while(exp_list_begin != exp_list.end()){
    result_ty = (*exp_list_begin)->SemAnalyze(venv, tenv, labelcount, errormsg);
    exp_list_begin ++;
  }

  return result_ty;
}

type::Ty *AssignExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* var_ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg);
  
  if(typeid(*var_) == typeid(absyn::SimpleVar)){
    env::EnvEntry* var_entry = venv->Look((static_cast<SimpleVar *>(var_))->sym_);
    if(typeid(*var_entry) == typeid(env::VarEntry)){
      // 不能修改loop循环中的变量
      if(static_cast<env::VarEntry *>(var_entry)->readonly_ == true){
        errormsg->Error(pos_, "loop variable can't be assigned");
      }
    }
    else{
      errormsg->Error(pos_, "assign lvalue is not a var");
    }
  }

  type::Ty* exp_ty = exp_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!var_ty->IsSameType(exp_ty)){
    errormsg->Error(pos_, "unmatched assign exp");
  }

  return type::VoidTy::Instance();
}

type::Ty *IfExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* test_ty = test_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!test_ty->IsSameType(type::IntTy::Instance())){
    errormsg->Error(pos_, "the condition must be an int");
  }
  type::Ty* then_ty = then_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(elsee_ != nullptr){
    type::Ty* else_ty = elsee_->SemAnalyze(venv, tenv, labelcount, errormsg);
    if(!then_ty->IsSameType(else_ty)){
      errormsg->Error(pos_, "then exp and else exp type mismatch");
    }
  }else{
    if(!then_ty->IsSameType(type::VoidTy::Instance())){
      errormsg->Error(pos_, "if-then exp's body must produce no value");
    }
  }
  return then_ty->ActualTy();
}

type::Ty *WhileExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* condition_ty = test_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!condition_ty->IsSameType(type::IntTy::Instance())){
    errormsg->Error(pos_, "the condition must be an int");
  }
  //进入body后需要将labelcount+1
  type::Ty* body_ty = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg)->ActualTy();
  if(typeid(*body_ty) != typeid(type::VoidTy)){
    errormsg->Error(pos_, "while body must produce no value");
  }
  return type::VoidTy::Instance();
}

type::Ty *ForExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  type::Ty* lo_ty = lo_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty* hi_ty = hi_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if(!(lo_ty->IsSameType(type::IntTy::Instance()) && hi_ty->IsSameType(type::IntTy::Instance()))){
    errormsg->Error(pos_, "for exp's range type is not integer");
  }
  //循环中的变量不能被修改
  venv->Enter(var_, new env::VarEntry(type::IntTy::Instance(), true));
  type::Ty* body_ty = body_->SemAnalyze(venv, tenv, labelcount + 1, errormsg);
  if(!body_ty->IsSameType(type::VoidTy::Instance())){
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
  for(Dec *dec : decs_->GetList()){
    dec->SemAnalyze(venv, tenv, labelcount, errormsg);
  }
  type::Ty* result;
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
  type::Ty* arr_ty = tenv->Look(typ_)->ActualTy();
  if(arr_ty == nullptr){
    errormsg->Error(pos_, "%s is not defined", typ_->Name().data());
  }
  if(typeid(*arr_ty) != typeid(type::ArrayTy)){
    errormsg->Error(pos_, "%s is not an array", typ_->Name().data());
  }

  type::Ty* arr_element_ty = (static_cast<type::ArrayTy *>(arr_ty))->ty_->ActualTy();
  type::Ty* size_ty = size_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  type::Ty* init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();

  if(!size_ty->IsSameType(type::IntTy::Instance())){
    errormsg->Error(pos_, "size must be a int");
  }
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
  std::list<absyn::FunDec *> fun_decs = functions_->GetList();
  auto fun_dec_begin = fun_decs.begin();
  while(fun_dec_begin != fun_decs.end()){
    if(venv->Look((*fun_dec_begin)->name_) != nullptr){
      errormsg->Error(pos_, "two functions have the same name");
      return;
    }
    type::TyList* formals = (*fun_dec_begin)->params_->MakeFormalTyList(tenv, errormsg);
    type::Ty* result_ty = type::VoidTy::Instance();
    if((*fun_dec_begin)->result_ != nullptr){
      result_ty = tenv->Look((*fun_dec_begin)->result_)->ActualTy();
      if(result_ty == nullptr){
        errormsg->Error(pos_, "return type not exist");
        return;
      }
    }
    venv->Enter((*fun_dec_begin)->name_, new env::FunEntry(formals, result_ty));
    fun_dec_begin ++;
  }
  fun_dec_begin = fun_decs.begin();
  while(fun_dec_begin != fun_decs.end()){
    venv->BeginScope();
    env::FunEntry* fun_entry = reinterpret_cast<env::FunEntry *>(venv->Look((*fun_dec_begin)->name_));
    std::list<type::Ty *> formal_list = fun_entry->formals_->GetList();
    std::list<absyn::Field *> param_list = (*fun_dec_begin)->params_->GetList();
    auto formal_begin = formal_list.begin();
    auto param_begin = param_list.begin();
    while(formal_begin != formal_list.end()){
      venv->Enter((*param_begin)->name_, new env::VarEntry(*formal_begin, false));
      formal_begin ++;
      param_begin ++;
    }
    type::Ty* body_ty = (*fun_dec_begin)->body_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
    if(!body_ty->IsSameType(fun_entry->result_)){
      errormsg->Error(pos_, "procedure returns value");
    }
    venv->EndScope();
    fun_dec_begin ++;
  }

}

void VarDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                        err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* init_ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg)->ActualTy();
  if(typ_ == nullptr){
    if(typeid(*init_ty) == typeid(type::NilTy)){
      errormsg->Error(pos_, "init should not be nil without type specified");
    }
    venv->Enter(var_, new env::VarEntry(init_ty));
  }else{
    type::Ty* type_ty = tenv->Look(typ_);
    if(type_ty == nullptr){
      errormsg->Error(pos_, "this type is not defined");
      venv->Enter(var_, new env::VarEntry(init_ty));
      return;
    }
    if(!type_ty->IsSameType(init_ty)){
      errormsg->Error(pos_, "type mismatch");
    }
    venv->Enter(var_, new env::VarEntry(type_ty));
  }
}

void TypeDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                         err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  std::list<absyn::NameAndTy *> type_dec_list = types_->GetList();
  auto type_dec_begin = type_dec_list.begin();
  while(type_dec_begin != type_dec_list.end()){
    if(tenv->Look((*type_dec_begin)->name_) != nullptr){
      errormsg->Error(pos_, "two types have the same name");
    }
    tenv->Enter((*type_dec_begin)->name_, new type::NameTy((*type_dec_begin)->name_, nullptr));
    type_dec_begin ++;
  }
  type_dec_begin = type_dec_list.begin();
  while(type_dec_begin != type_dec_list.end()){
    type::NameTy* name_ty = reinterpret_cast<type::NameTy *>(tenv->Look((*type_dec_begin)->name_));
    name_ty->ty_ = (*type_dec_begin)->ty_->SemAnalyze(tenv, errormsg);
    type_dec_begin ++;
  }
  bool is_recursive = false;
  type_dec_begin = type_dec_list.begin();
  while(type_dec_begin != type_dec_list.end()){
    type::NameTy* name_ty = reinterpret_cast<type::NameTy *>(tenv->Look((*type_dec_begin)->name_));
    type::NameTy* name_ty_ptr = name_ty;
    while(true){
      type::Ty* next_ty = name_ty_ptr->ty_;
      if(typeid(*next_ty) != typeid(type::NameTy)){
        break; 
      }else if(reinterpret_cast<type::NameTy *>(next_ty)->sym_ == name_ty->sym_){
        is_recursive = true;
        break;
      }
      name_ty_ptr = reinterpret_cast<type::NameTy *>(next_ty);
    }
    if(is_recursive){
      errormsg->Error(pos_, "illegal type cycle");
      break;
    }
    type_dec_begin++;
  }
}

type::Ty *NameTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* name_ty = tenv->Look(name_);
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