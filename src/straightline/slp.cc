#include "straightline/slp.h"

#include <iostream>
#include <algorithm>
namespace A {
int A::CompoundStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return std::max(this->stm1->MaxArgs(),this->stm2->MaxArgs());
}

Table *A::CompoundStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return this->stm2->Interp(this->stm1->Interp(t));
}

int A::AssignStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return this->exp->MaxArgs();
}

Table *A::AssignStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable *tempIntAndTable = this->exp->Interp(t);
  Table *oldTable = tempIntAndTable->t;
  Table *newTable = oldTable->Update(this->id,tempIntAndTable->i);
  return newTable;
}

int A::PrintStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return this->exps->MaxArgs();
}

Table *A::PrintStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  IntAndTable *tempIntAndTable = this->exps->Interp(t);
  return tempIntAndTable->t;
}

int A::IdExp::MaxArgs() const {
  return 1;
}

IntAndTable *A::IdExp::Interp(Table *t) const {
  int value_of_Id = t->Lookup(this->id);
  IntAndTable *result = new IntAndTable(value_of_Id,t);
  return result;
}

int A::NumExp::MaxArgs() const {
  return 1;
}

IntAndTable *A::NumExp::Interp(Table *t) const {
  IntAndTable *result = new IntAndTable(this->num,t);
  return result;
}

int A::OpExp::MaxArgs() const {
  return 1;
} 

IntAndTable *A::OpExp::Interp(Table *t) const {
  IntAndTable *LeftAndTable = this->left->Interp(t);
  IntAndTable *RightAndTable = this->right->Interp(t);
  if(this->oper == BinOp::PLUS){
    return new IntAndTable(LeftAndTable->i+RightAndTable->i,t);
  }else if(this->oper == BinOp::MINUS){
    return new IntAndTable(LeftAndTable->i-RightAndTable->i,t);
  }else if(this->oper == BinOp::TIMES){
    return new IntAndTable(LeftAndTable->i * RightAndTable->i,t);
  }else if(this->oper == BinOp::DIV){
    return new IntAndTable(LeftAndTable->i % RightAndTable->i,t);
  }
}

int A::EseqExp::MaxArgs() const {
  return std::max(this->stm->MaxArgs(),this->exp->MaxArgs());
}

IntAndTable *A::EseqExp::Interp(Table *t) const {
  return this->exp->Interp(this->stm->Interp(t));
}

int A::PairExpList::MaxArgs() const {
  return this->exp->MaxArgs() + this->tail->MaxArgs();
}

IntAndTable *A::PairExpList::Interp(Table* t) const {
  IntAndTable *expAndTable = this->exp->Interp(t);
  printf("%d ", expAndTable->i);
  return this->tail->Interp(expAndTable->t);
}

int A::LastExpList::MaxArgs() const {
  return this->exp->MaxArgs();
}

IntAndTable *A::LastExpList::Interp(Table *t) const {
  IntAndTable *result = this->exp->Interp(t);
  printf("%d\n", result->i);
  return result;
}

int Table::Lookup(const std::string &key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(const std::string &key, int val) const {
  return new Table(key, val, this);
}
}  // namespace A
