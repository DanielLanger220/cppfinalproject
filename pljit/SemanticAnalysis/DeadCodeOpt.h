#ifndef PLJIT_DEADCODEOPT_H
#define PLJIT_DEADCODEOPT_H

#include "OptimisePass.h"

namespace jit {

class DeadCodeOpt : public OptimisePass {


    public:

    // Constructor
    DeadCodeOpt() = default;


    // The visit methods to support the visitor pattern
    void visit(AstLiteral& node) override;
    void visit(AstIdentifier& node) override ;
    void visit(AstUnaryArithmeticExpression& node) override ;
    void visit(AstBinaryArithmeticExpression& node) override;
    void visit(AstReturn& node) override ;
    void visit(AstAssignment& node) override ;
    void visit(AstStatementList& node) override;
    void visit(AstFunction& node) override;

};

} // namespace jit

#endif //PLJIT_DEADCODEOPT_H
