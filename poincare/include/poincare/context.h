#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

namespace Poincare {

class Expression;
class SymbolAbstract;

class Context {
public:
  virtual const Expression expressionForSymbol(const SymbolAbstract & symbol, bool clone) = 0;
  virtual void setExpressionForSymbol(const Expression & expression, const SymbolAbstract & symbol, Context & context) = 0;
};

}

#endif
