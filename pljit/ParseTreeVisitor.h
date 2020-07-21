#ifndef PLJIT_PARSETREEVISITOR_H
#define PLJIT_PARSETREEVISITOR_H


namespace jit {

class TreeNode;
class IdentifierNode;
class LiteralNode;
class GenericTerminalNode;
class PrimaryExprNode;
class UnaryExprNode;
class MultExprNode;
class AdditiveExprNode;
class AssignExprNode;
class Statement;
class StatementList;
class CompoundStatement;
class ParamDeclNode;
class VarDeclNode;
class ConstDeclNode;
class DeclListNode;
class InitDeclNode;
class InitDeclListNode;
class FuncDeclNode;


class ParseTreeVisitor {

    public:

    virtual void visit(const IdentifierNode& node) = 0;
    virtual void visit(const LiteralNode& node) = 0;
    virtual void visit(const GenericTerminalNode& node) = 0;
    virtual void visit(const PrimaryExprNode& node) = 0;
    virtual void visit(const UnaryExprNode& node) = 0;
    virtual void visit(const MultExprNode& node) = 0;
    virtual void visit(const AdditiveExprNode& node) = 0;
    virtual void visit(const AssignExprNode& node) = 0;
    virtual void visit(const Statement& node) = 0;
    virtual void visit(const StatementList& node) = 0;
    virtual void visit(const CompoundStatement& node) = 0;
    virtual void visit(const ParamDeclNode& node) = 0;
    virtual void visit(const VarDeclNode& node) = 0;
    virtual void visit(const ConstDeclNode& node) = 0;
    virtual void visit(const DeclListNode& node) = 0;
    virtual void visit(const InitDeclNode& node) = 0;
    virtual void visit(const InitDeclListNode& node) = 0;
    virtual void visit(const FuncDeclNode& node) = 0;
};

} // namespace jit

#endif //PLJIT_PARSETREEVISITOR_H
