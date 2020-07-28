#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H

#include <optional>
#include <cstdint>
#include <memory>

#include "pljit/SemanticAnalysis/SymbolTable.h"
#include "pljit/SourceCodeManager.h"
#include "AstVisitor.h"

namespace jit {


class AstNode {

    public:

    enum class AstType {
        AstStatement,
        AstArithmeticExpression,
    };

    // Constructor
    AstNode(SourceCodeReference location, AstType type) : location{location}, type{type} {}

    // Desctructor
    virtual ~AstNode() = default;

    // Virtual accept method to support the visitor pattern
    virtual void accept(AstVisitor& v) = 0;

    SourceCodeReference location;       // Reference to the source code of this node
    const AstType type;                 // Specifies the general type of the AstNode

};


class AstArithmeticExpression : public AstNode {

    public:

    enum class Subtype {
        Literal,
        Identifier,
        Unary,
        Binary
    };

    // Constructor
    AstArithmeticExpression(SourceCodeReference location, Subtype subtype) : AstNode{location, AstNode::AstType::AstArithmeticExpression}, subtype{subtype} {}

    //virtual std::optional<int64_t> evaluate(const SourceCodeManager& manager, const SymbolTable& table) = 0;

    const Subtype subtype{};        // Specifies the type of the arithmetic expression

};

class AstLiteral : public AstArithmeticExpression {

    public:

    AstLiteral(SourceCodeReference location, int64_t value) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Literal} , value{value} {}

    void accept(AstVisitor& v) override {v.visit(*this);}

    const int64_t value{};

};

class AstIdentifier : public AstArithmeticExpression {

    public:

    // Constructor
    AstIdentifier(SourceCodeReference location, size_t index) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Identifier} , index{index} {}

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}


    const size_t index{};           // Index of this Identifier in the symbol table
};


class AstBinaryArithmeticExpression : public AstArithmeticExpression {

    public:

    enum class ArithmeticOperation {
        Plus,
        Minus,
        Mul,
        Div
    };

    // Constructor
    AstBinaryArithmeticExpression(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> lhs, std::unique_ptr<AstArithmeticExpression> rhs, ArithmeticOperation op) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Binary},
                                                                                                                                                              lhs{std::move(lhs)},
                                                                                                                                                              rhs{std::move(rhs)}, op{op}{}
    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> lhs{};         // Left hand side expression of this expression
    std::unique_ptr<AstArithmeticExpression> rhs{};         // Right hand side expression of this expression
    const ArithmeticOperation op{};                         // Specifies the arithmetic operation (+, -, *, /) of this expression

};


class AstUnaryArithmeticExpression : public AstArithmeticExpression {

    public:

    // Constructor
    AstUnaryArithmeticExpression(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> subexpr) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Unary},
                                                                                                                   subexpr{std::move(subexpr)} {}
    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> subexpr{};     // The subexpression of this expression (expr = -subexpr)

};


class AstStatement : public AstNode {

    public:

    enum class SubType {
        AstAssignment,
        AstReturn
    };

    // Constructor
    AstStatement(SourceCodeReference location, SubType subtype) : AstNode{location, AstNode::AstType::AstStatement} , subtype{subtype} {}

    //virtual void execute(const SourceCodeManager& manager, const SymbolTable& table) = 0;

    const SubType subtype;          // Specifies the type of the Statement

};


class AstAssignment : public AstStatement {

    public:

    // Constructor
    AstAssignment(SourceCodeReference location, std::unique_ptr<AstIdentifier> lhs, std::unique_ptr<AstArithmeticExpression> rhs) : AstStatement{location, AstStatement::SubType::AstAssignment},
                                                                                                                                    lhs{std::move(lhs)},
                                                                                                                                    rhs{std::move(rhs)} {}
    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstIdentifier> lhs{};
    std::unique_ptr<AstArithmeticExpression> rhs{};

};

class AstReturn : public AstStatement {

    public:

    // Constructor
    AstReturn(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> returnvalue) : AstStatement{location, AstStatement::SubType::AstReturn},
                                                                                                    returnvalue{std::move(returnvalue)} {}

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> returnvalue{};         // The expression to be returned
};


class AstFunction {

    public:

    // Constructor
    explicit AstFunction(std::vector<std::unique_ptr<AstStatement>> statements) : statements{std::move(statements)} {}

    std::vector<std::unique_ptr<AstStatement>> statements;      // Contains the statements of the function
};


} // namespace jit

#endif //PLJIT_ASTNODE_H
