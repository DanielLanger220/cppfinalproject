#ifndef PLJIT_CONSTANTPROPOPT_H
#define PLJIT_CONSTANTPROPOPT_H

#include <vector>
#include <map>
#include <memory>
#include <optional>

#include "OptimisePass.h"
#include "AstNode.h"


namespace jit {

// ConstantPropOpt                      Performs a Constant-Propagation optimisation on an Ast
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

    // Maps an AstNode (its address) to an optional<int64_t> value.
    // nullopt        ==> The AstNode is currently marked as non-constant
    // int64_t value  ==> The AstNode is currently marked as constant with the specified integer value
    std::map<AstNode*, std::optional<int64_t>> exprmap{};

    // For all identifiers (parameters and variables), if they are currently marked as constant,
    // stores their current values in order of the index that was given to the identifiers during the sem. analysis
    // [P1, P2, ... , V1, V2, ...]
    std::vector<std::optional<int64_t>> vartable{};

    bool firstRun{true};        // The optimisation is done in two runs over the nodes. This flag specifies for the visit methods, which run should be performed

};

} // namespace jit

#endif //PLJIT_CONSTANTPROPOPT_H
