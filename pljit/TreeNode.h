#ifndef PLJIT_TREENODE_H
#define PLJIT_TREENODE_H

#include <memory>
#include <vector>

#include "SourceCodeManager.h"
#include "ParseTreeVisitor.h"

namespace jit {


// Base class for all nodes of the parse tree
class TreeNode {

    public:

    enum class Type {
        Identifier,
        Literal,
        GenericTerminal,
        PrimaryExpr,
        UnaryExpr,
        MultExpr,
        AdditiveExpr,
        AssignExpr,
        Statement,
        StatementList,
        CompundStatement,
        ParamDecl,
        VarDecl,
        ConstDecl,
        DeclList,
        FuncDecl
    };

    // Constructor
    TreeNode(Type nodetype, SourceCodeReference location) : nodetype{nodetype}, location{location} {}

    // Destructor
    virtual ~TreeNode() = default;

    virtual void accept(ParseTreeVisitor& visitor) const = 0;

    const Type nodetype;                    // Specifies the type of the parse tree node
    const SourceCodeReference location;     // Reference to the location in source code

};


class NonTerminalTreeNode : public TreeNode {

};


class IdentifierNode : public TreeNode {

    public:

    // Constructor
    IdentifierNode(SourceCodeReference location, std::string_view id) : TreeNode{TreeNode::Type::Identifier, location}, id{id} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    std::string_view id;

};

class LiteralNode : public TreeNode {

    public:

    // Constructor
    LiteralNode(SourceCodeReference location, int64_t value) : TreeNode{TreeNode::Type::Literal, location}, value{value} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    int64_t value;

};

class GenericTerminalNode : public TreeNode {

    public:

    // Constructor
    explicit GenericTerminalNode(SourceCodeReference location) : TreeNode{TreeNode::Type::GenericTerminal, location} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};


class PrimaryExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Identifier,
        Literal,
        AdditiveExpr
    };

    PrimaryExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::PrimaryExpr, location},
                                                                                                                    subtype{subtype}, nodes{std::move(nodes)} {}
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;
    std::vector<std::unique_ptr<TreeNode>> nodes;

};


class UnaryExprNode : public TreeNode {

    public:

    UnaryExprNode(bool sign, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::UnaryExpr, location},
                                                                                                               sign{sign}, nodes{std::move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    bool sign{false};
    std::vector<std::unique_ptr<TreeNode>> nodes;

};

class MultExprNode : public TreeNode {

    public:

    enum class SubType {
        Unary,
        Binary
    };

    // Constructor
    MultExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::MultExpr, location},
                                                                                                                subtype{subtype}, nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;
    std::vector<std::unique_ptr<TreeNode>> nodes;

};


class AdditiveExprNode : public TreeNode {

    public:

    enum class SubType {
        Unary,
        Binary
    };

    // Constructor
    AdditiveExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::AdditiveExpr, location},
                                                                                                                   subtype{subtype}, nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;
    std::vector<std::unique_ptr<TreeNode>> nodes;


};

class AssignExprNode : public TreeNode {

    public:

    // Constructor
    AssignExprNode(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::AssignExpr, location},
                                                                                                 nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    std::vector<std::unique_ptr<TreeNode>> nodes;

};

class Statement : public TreeNode {

    public:

    enum class SubType {
        Return,
        Assign
    };


    // Constructor
    Statement(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::Statement, location},
                                                                                                             subtype{subtype}, nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;
    std::vector<std::unique_ptr<TreeNode>> nodes;
};

class StatementList : public TreeNode {

    public:

    // Constructor
    StatementList(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::StatementList, location},
                                                                                                nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    std::vector<std::unique_ptr<TreeNode>> nodes;

};

class CompoundStatement : public TreeNode {

    public:

    // Constructor
    CompoundStatement(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : TreeNode{TreeNode::Type::CompundStatement, location},
    nodes{move(nodes)} {}

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    std::vector<std::unique_ptr<TreeNode>> nodes;

};


class ParamDeclNode : public TreeNode {
    public:

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

class VarDeclNode : public TreeNode {

    public:

    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class ConstDeclNode : public TreeNode {

    public:
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

class DeclListNode : public TreeNode {

    public:
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};







class FuncDeclNode : public TreeNode {

    public:

    std::unique_ptr<ParamDeclNode> paramdecl{nullptr};
    std::unique_ptr<VarDeclNode> vardecl{nullptr};
    std::unique_ptr<ConstDeclNode> constdecl{nullptr};

    std::unique_ptr<CompoundStatement> compstatement;


    private:
    bool parseParamDecl();
    bool parseVarDecl();
    bool ParseConstDecl();

};


} // namespace jit

#endif //PLJIT_TREENODE_H
