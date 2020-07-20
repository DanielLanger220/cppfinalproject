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

    // accept               accept method for the visitor pattern
    virtual void accept(ParseTreeVisitor& visitor) const = 0;

    const Type nodetype;                    // Specifies the type of the parse tree node
    const SourceCodeReference location;     // Reference to the location in source code

};

// Base class for all non terminal nodes of the parse tree
class NonTerminalTreeNode : public TreeNode {

    public:
    NonTerminalTreeNode(Type nodetype, SourceCodeReference location, std::vector<std::unique_ptr<TreeNode>> nodes) : TreeNode{nodetype, location}, nodes{std::move(nodes)} {}

    std::vector<std::unique_ptr<TreeNode>> nodes{};
};


class IdentifierNode : public TreeNode {

    public:

    // Constructor
    IdentifierNode(SourceCodeReference location, std::string_view id) : TreeNode{TreeNode::Type::Identifier, location}, id{id} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    std::string_view id;

};

class LiteralNode : public TreeNode {

    public:

    // Constructor
    LiteralNode(SourceCodeReference location, int64_t value) : TreeNode{TreeNode::Type::Literal, location}, value{value} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    int64_t value;

};

class GenericTerminalNode : public TreeNode {

    public:

    enum class SubType {
        Plus,
        Minus,
        Mul,
        Div,
        Other
    };

    // Constructor
    explicit GenericTerminalNode(SourceCodeReference location) : TreeNode{TreeNode::Type::GenericTerminal, location} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};


class PrimaryExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Identifier,
        Literal,
        AdditiveExpr
    };

    // Constructor
    PrimaryExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::PrimaryExpr, location, std::move(nodes)},
                                                                                                                   subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Primary Expression
};


class UnaryExprNode : public NonTerminalTreeNode {

    public:

    UnaryExprNode(bool sign, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::UnaryExpr, location, std::move(nodes)},
                                                                                                           sign{sign} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    bool sign{false};

};

class MultExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Unary,
        Binary
    };

    // Constructor
    MultExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::MultExpr, location, std::move(nodes)},
                                                                                                                subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Multiplicative Expression

};


class AdditiveExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Unary,
        Binary
    };

    // Constructor
    AdditiveExprNode(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::AdditiveExpr, location, std::move(nodes)},
                                                                                                                   subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Additive Expression
};

class AssignExprNode : public NonTerminalTreeNode {

    public:

    // Constructor
    AssignExprNode(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::AssignExpr, location, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class Statement : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Return,
        Assign
    };

    // Constructor
    Statement(SubType subtype, std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::Statement, location, std::move(nodes)},
                                                                                                             subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Statement
};

class StatementList : public NonTerminalTreeNode {

    public:

    // Constructor
    StatementList(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::StatementList, location, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class CompoundStatement : public NonTerminalTreeNode {

    public:

    // Constructor
    CompoundStatement(std::vector<std::unique_ptr<TreeNode>> nodes, SourceCodeReference location) : NonTerminalTreeNode{TreeNode::Type::CompundStatement, location, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};


class ParamDeclNode : public TreeNode {
    public:

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class VarDeclNode : public TreeNode {

    public:

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class ConstDeclNode : public TreeNode {

    public:

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

class DeclListNode : public TreeNode {

    public:

    // accept               accept method for the visitor pattern
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
