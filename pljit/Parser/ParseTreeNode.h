#ifndef PLJIT_PARSETREENODE_H
#define PLJIT_PARSETREENODE_H

#include <memory>
#include <vector>
#include <optional>

#include "pljit/Parser/ParseTreeVisitor.h"
#include "pljit/SourceCodeManager.h"

namespace jit {


// Base class for all nodes of the parse tree
class ParseTreeNode {

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
        InitDeclList,
        InitDecl,
        FuncDecl
    };

    // Constructor
    ParseTreeNode(SourceCodeReference location, Type nodetype) : location{location}, nodetype{nodetype} {}

    // Destructor
    virtual ~ParseTreeNode() = default;

    // accept               accept method for the visitor pattern
    virtual void accept(ParseTreeVisitor& visitor) const = 0;

    const SourceCodeReference location;     // Reference to the location in source code
    const Type nodetype;                    // Specifies the type of the parse tree node


};

// Base class for all non terminal nodes of the parse tree
class NonTerminalTreeNode : public ParseTreeNode {

    public:
    NonTerminalTreeNode(SourceCodeReference location, Type nodetype, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : ParseTreeNode{location, nodetype}, nodes{std::move(nodes)} {}

    std::vector<std::unique_ptr<ParseTreeNode>> nodes{};
};


class IdentifierNode : public ParseTreeNode {

    public:

    // Constructor
    explicit IdentifierNode(SourceCodeReference location) : ParseTreeNode{location, ParseTreeNode::Type::Identifier} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

class LiteralNode : public ParseTreeNode {

    public:

    // Constructor
    LiteralNode(SourceCodeReference location, int64_t value) : ParseTreeNode{location, ParseTreeNode::Type::Literal}, value{value} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    int64_t value;

};

class GenericTerminalNode : public ParseTreeNode {

    public:

    enum class SubType {
        Plus,
        Minus,
        Mul,
        Div,
        Other
    };

    // Constructor
    explicit GenericTerminalNode(SourceCodeReference location, SubType subtype) : ParseTreeNode{location, ParseTreeNode::Type::GenericTerminal}, subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;
};


class PrimaryExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Identifier,
        Literal,
        AdditiveExpr
    };

    // Constructor
    PrimaryExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, SubType subtype) : NonTerminalTreeNode{location, ParseTreeNode::Type::PrimaryExpr, std::move(nodes)},
                                                                                                                   subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Primary Expression
};


class UnaryExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Plus,
        Minus,
        NoSign
    };

    UnaryExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, SubType subtype) : NonTerminalTreeNode{location, ParseTreeNode::Type::UnaryExpr, std::move(nodes)},
                                                                                                           subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype{SubType::NoSign};

};

class MultExprNode : public NonTerminalTreeNode {

    public:

    enum class SubType {
        Unary,
        Binary
    };

    // Constructor
    MultExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, SubType subtype) : NonTerminalTreeNode{location, ParseTreeNode::Type::MultExpr, std::move(nodes)},
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
    AdditiveExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, SubType subtype) : NonTerminalTreeNode{location, ParseTreeNode::Type::AdditiveExpr, std::move(nodes)},
                                                                                                                   subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Additive Expression
};

class AssignExprNode : public NonTerminalTreeNode {

    public:

    // Constructor
    AssignExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::AssignExpr, std::move(nodes)} {}

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
    Statement(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, SubType subtype) : NonTerminalTreeNode{location, ParseTreeNode::Type::Statement, std::move(nodes)},
                                                                                                             subtype{subtype} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const SubType subtype;          // Indicates the subtype of the Statement
};

class StatementList : public NonTerminalTreeNode {

    public:

    // Constructor
    StatementList(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::StatementList, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class CompoundStatement : public NonTerminalTreeNode {

    public:

    // Constructor
    CompoundStatement(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::CompundStatement, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};


class ParamDeclNode : public NonTerminalTreeNode {

    public:

    ParamDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class VarDeclNode : public NonTerminalTreeNode {

    public:

    VarDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class ConstDeclNode : public NonTerminalTreeNode {

    public:

    ConstDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

class DeclListNode : public NonTerminalTreeNode {

    public:

    DeclListNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}


    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

class InitDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    InitDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}


};

class InitDeclListNode : public NonTerminalTreeNode {

    public:

    // Constructor
    InitDeclListNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};




class FuncDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    FuncDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, bool hasParam, bool hasVar, bool hasConst) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)},
                                                                                                                                          hasParamDecl{hasParam}, hasVarDecl{hasVar}, hasConstDecl{hasConst}{}
    const ParamDeclNode* getParameterDeclarations() const;
    const VarDeclNode* getVariableDeclarations() const;
    const ConstDeclNode* getConstantDeclarations() const;

    const StatementList* getStatements() const;

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const bool hasParamDecl{false};
    const bool hasVarDecl{false};
    const bool hasConstDecl{false};

    private:


};
} // namespace jit

#endif //PLJIT_PARSETREENODE_H
