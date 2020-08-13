#ifndef PLJIT_PARSETREENODE_H
#define PLJIT_PARSETREENODE_H

#include <memory>
#include <vector>
#include <optional>

#include "pljit/CodeManagement/SourceCodeManager.h"
#include "pljit/Parser/ParseTreeVisitor.h"

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


// Class to represent identifier nodes
class IdentifierNode : public ParseTreeNode {

    public:

    // Constructor
    explicit IdentifierNode(SourceCodeReference location) : ParseTreeNode{location, ParseTreeNode::Type::Identifier} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent literal nodes
class LiteralNode : public ParseTreeNode {

    public:

    // Constructor
    LiteralNode(SourceCodeReference location, int64_t value) : ParseTreeNode{location, ParseTreeNode::Type::Literal}, value{value} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const int64_t value;        // the value the literal represents in the source code

};

// Class to represent generic nodes, i.e. arithmetic operators, separators and keywords
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

// Class to represent primary expression nodes
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

// Class to represent unary arithmetic expression nodes
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

// Class to represent multiplicative expression nodes
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

// Class to represent additive expression nodes
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

// Class to represent assignment expression nodes
class AssignExprNode : public NonTerminalTreeNode {

    public:

    // Constructor
    AssignExprNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::AssignExpr, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent statement nodes
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

// Class to represent statement-list nodes
class StatementList : public NonTerminalTreeNode {

    public:

    // Constructor
    StatementList(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::StatementList, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent compound statement nodes
class CompoundStatement : public NonTerminalTreeNode {

    public:

    // Constructor
    CompoundStatement(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::CompundStatement, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent parameter-declaration nodes
class ParamDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    ParamDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent variable-declaration nodes
class VarDeclNode : public NonTerminalTreeNode {

    public:

    VarDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent constant-declaration nodes
class ConstDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    ConstDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

// Class to represent declaration-list nodes
class DeclListNode : public NonTerminalTreeNode {

    public:

    DeclListNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}


    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}
};

// Class to represent init-declaration nodes
class InitDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    InitDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};

// Class to represent init-declaration-list nodes
class InitDeclListNode : public NonTerminalTreeNode {

    public:

    // Constructor
    InitDeclListNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)} {}

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

};


// Class to represent function-declaration nodes
class FuncDeclNode : public NonTerminalTreeNode {

    public:

    // Constructor
    FuncDeclNode(SourceCodeReference location, std::vector<std::unique_ptr<ParseTreeNode>> nodes, bool hasParam, bool hasVar, bool hasConst) : NonTerminalTreeNode{location, ParseTreeNode::Type::InitDecl, std::move(nodes)},
                                                                                                                                          hasParamDecl{hasParam}, hasVarDecl{hasVar}, hasConstDecl{hasConst}{}

    // getParameterDeclarations                 Returns a pointer to the parameter-declaration node
    const ParamDeclNode* getParameterDeclarations() const;

    // getVariableDeclarations                  Returns a pointer to the variable-declaration node
    const VarDeclNode* getVariableDeclarations() const;

    // getConstantDeclarations                  Returns a pointer to the constant-declaration node
    const ConstDeclNode* getConstantDeclarations() const;

    // getStatements                            Returns a pointer to the statement-list node
    const StatementList* getStatements() const;

    // accept               accept method for the visitor pattern
    void accept(ParseTreeVisitor& visitor) const override { visitor.visit(*this);}

    const bool hasParamDecl{false};         // Indicates, if the function has parameter declarations
    const bool hasVarDecl{false};           // Indicates, if the function has variable declarations
    const bool hasConstDecl{false};         // Indicates, if the function has constant declarations

};


} // namespace jit

#endif //PLJIT_PARSETREENODE_H
