#ifndef PLJIT_DEADCODEOPT_H
#define PLJIT_DEADCODEOPT_H

#include "OptimisePass.h"

namespace jit {

class DeadCodeOpt : public OptimisePass {

    public:

    // Constructor
    DeadCodeOpt() = default;

    // The visit methods to support the visitor pattern
    void visit(AstLiteral&) override {};
    void visit(AstIdentifier&) override {};
    void visit(AstUnaryArithmeticExpression&) override {};
    void visit(AstBinaryArithmeticExpression&) override {};
    void visit(AstReturn&) override {};
    void visit(AstAssignment&) override {};
    void visit(AstStatementList& node) override;
    void visit(AstFunction& node) override;

};

} // namespace jit

#endif //PLJIT_DEADCODEOPT_H
