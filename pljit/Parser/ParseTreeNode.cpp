#include "ParseTreeNode.h"

using namespace std;

namespace jit {

const ParamDeclNode* FuncDeclNode::getParameterDeclarations() const {

    return hasParamDecl ?  static_cast<ParamDeclNode*>(nodes[0].get()) : nullptr;
}

const VarDeclNode* FuncDeclNode::getVariableDeclarations() const {

    if (!hasVarDecl)
        return nullptr;

    unsigned index = hasParamDecl ? 1 : 0;
    return static_cast<VarDeclNode*>(nodes[index].get());
}

const ConstDeclNode* FuncDeclNode::getConstantDeclarations() const {

    if (!hasConstDecl)
        return nullptr;

    unsigned index = (hasParamDecl ? 1 : 0) + (hasVarDecl ? 1 : 0);
    return static_cast<ConstDeclNode*>(nodes[index].get());
}

const StatementList* FuncDeclNode::getStatements() const {

    unsigned index = (hasParamDecl ? 1 : 0) + (hasVarDecl ? 1 : 0) + (hasConstDecl ? 1 : 0);

    CompoundStatement& compstatement = static_cast<CompoundStatement&>(*nodes[index]);

    if (compstatement.nodes[1] == nullptr)
        return nullptr;

    return static_cast<StatementList*>(compstatement.nodes[1].get());

}

} // namespace jit