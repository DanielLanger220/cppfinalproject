#ifndef PLJIT_CONSTANTPROPOPT_H
#define PLJIT_CONSTANTPROPOPT_H

#include <vector>
#include <map>
#include <memory>
#include <optional>

#include "OptimisePass.h"
#include "AstNode.h"


namespace jit {

class ConstantPropOpt : public OptimisePass {


    public:

    // Constructor
    ConstantPropOpt() = default;


    // The visit methods to support the visitor pattern
    void visit(AstLiteral& node) override;
    void visit(AstIdentifier& node) override ;
    void visit(AstUnaryArithmeticExpression& node) override ;
    void visit(AstBinaryArithmeticExpression& node) override;
    void visit(AstReturn& node) override ;
    void visit(AstAssignment& node) override ;
    void visit(AstStatementList& node) override;
    void visit(AstFunction& node) override;

    private:

    std::map<AstNode*, std::optional<int64_t>> exprmap{};
    std::vector<std::optional<int64_t>> vartable{};


    size_t varstart{};
    bool firstRun{true};

};

} // namespace jit
#endif //PLJIT_CONSTANTPROPOPT_H
