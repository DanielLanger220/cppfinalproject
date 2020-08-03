#ifndef PLJIT_ASTVISITOR_H
#define PLJIT_ASTVISITOR_H

namespace jit {


class AstLiteral;
class AstIdentifier;
class AstUnaryArithmeticExpression;
class AstBinaryArithmeticExpression;
class AstReturn;
class AstAssignment;
class AstStatementList;
class AstFunction;

class AstVisitor {

    public:

    virtual void visit(const AstLiteral& node) = 0;
    virtual void visit(const AstIdentifier& node) = 0;
    virtual void visit(const AstUnaryArithmeticExpression& node) = 0;
    virtual void visit(const AstBinaryArithmeticExpression& node) = 0;
    virtual void visit(const AstReturn& node) = 0;
    virtual void visit(const AstAssignment& node) = 0;
    virtual void visit(const AstStatementList& node) = 0;
    virtual void visit(const AstFunction& node) = 0;
};

} // namespace jit

#endif //PLJIT_ASTVISITOR_H
