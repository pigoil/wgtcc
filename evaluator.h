#ifndef _WGTCC_EVALUATOR_H_
#define _WGTCC_EVALUATOR_H_

#include "ast.h"
#include "error.h"
#include "visitor.h"


class Expr;

template<typename T>
class Evaluator: public Visitor {
public:
  Evaluator() {}

  virtual ~Evaluator() {}

  virtual void VisitBinaryOp(BinaryOp* binary);
  virtual void VisitUnaryOp(UnaryOp* unary);
  virtual void VisitConditionalOp(ConditionalOp* cond);
  
  virtual void VisitFuncCall(FuncCall* funcCall) {
    Error(funcCall, "expect constant expression");
  }

  virtual void VisitEnumerator(Enumerator* enumer) {
    val_ = static_cast<T>(enumer->Val());
  }

  virtual void VisitIdentifier(Identifier* ident) {
    Error(ident, "expect constant expression");
  }

  virtual void VisitObject(Object* obj) {
    Error(obj, "expect constant expression");
  }

  virtual void VisitASTConstant(ASTConstant* cons) {
    if (cons->type()->IsFloat()) {
      val_ = static_cast<T>(cons->fval());
    } else if (cons->type()->IsInteger()) {
      val_ = static_cast<T>(cons->ival());
    } else {
      assert(false);
    }
  }

  virtual void VisitTempVar(TempVar* tempVar) {
    assert(false);
  }

  T Eval(Expr* expr) {
    expr->Accept(this);
    return val_;
  }

private:
  T val_;
};


struct Addr {
  std::string label_;
  int offset_;
};

template<>
class Evaluator<Addr>: public Visitor {
public:
  Evaluator<Addr>() {}
  
  virtual ~Evaluator<Addr>() {}

  virtual void VisitBinaryOp(BinaryOp* binary);
  virtual void VisitUnaryOp(UnaryOp* unary);
  virtual void VisitConditionalOp(ConditionalOp* cond);
  
  virtual void VisitFuncCall(FuncCall* funcCall) {
    Error(funcCall, "expect constant expression");
  }

  virtual void VisitEnumerator(Enumerator* enumer) {
    addr_.offset_ = enumer->Val();
  }

  virtual void VisitIdentifier(Identifier* ident) {
    addr_.label_ = ident->Name();
    addr_.offset_ = 0;
  }

  virtual void VisitObject(Object* obj) {
    if (!obj->IsStatic()) {
      Error(obj, "expect static object");
    }
    addr_.label_ = obj->Repr();
    addr_.offset_ = 0;
  }

  virtual void VisitASTConstant(ASTConstant* cons);
  virtual void VisitTempVar(TempVar* tempVar) {
    assert(false);
  }

  Addr Eval(Expr* expr) {
    expr->Accept(this);
    return addr_;
  }

private:
  Addr addr_;
};

#endif
