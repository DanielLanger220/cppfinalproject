#ifndef PLJIT_OPTIMISEPASS_H
#define PLJIT_OPTIMISEPASS_H

namespace jit {

class AstNode;
class AstLiteral;
class AstIdentifier;
class AstUnaryArithmeticExpression;
class AstBinaryArithmeticExpression;
class AstReturn;
class AstAssignment;
class AstStatementList;
class AstFunction;

// OptimisePass                         Base class for optimisations on an Ast
class OptimisePass {

    public:

    virtual void visit(AstLiteral& node) = 0;
    virtual void visit(AstIdentifier& node) = 0;
    virtual void visit(AstUnaryArithmeticExpression& node) = 0;
    virtual void visit(AstBinaryArithmeticExpression& node) = 0;
    virtual void visit(AstReturn& node) = 0;
    virtual void visit(AstAssignment& node) = 0;
    virtual void visit(AstStatementList& node) = 0;
    virtual void visit(AstFunction& node) = 0;


};

} // namespace jit

#endif //PLJIT_OPTIMISEPASS_H
