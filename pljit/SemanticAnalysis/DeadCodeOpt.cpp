#include "DeadCodeOpt.h"

#include "AstNode.h"

namespace jit {

void DeadCodeOpt::visit(AstLiteral& node) {

}


void DeadCodeOpt::visit(AstIdentifier& node) {

}

void DeadCodeOpt::visit(AstUnaryArithmeticExpression& node) {


}

void DeadCodeOpt::visit(AstBinaryArithmeticExpression& node) {


}

void DeadCodeOpt::visit(AstReturn& node) {


}

void DeadCodeOpt::visit(AstAssignment& node) {


}

void DeadCodeOpt::visit(AstStatementList& node) {

    auto it = node.statements.begin();

    while((*it)->subtype != AstStatement::SubType::AstReturn)
        ++it;

    std::cout << "Daniel + Sophie!\n";

    ++it;

    node.statements.erase(it, node.statements.end());
}

void DeadCodeOpt::visit(AstFunction& node) {

    node.statementlist->optimise(*this);

}

} // namespace jit