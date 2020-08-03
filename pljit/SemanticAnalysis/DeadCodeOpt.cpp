#include "DeadCodeOpt.h"

#include "AstNode.h"

namespace jit {

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