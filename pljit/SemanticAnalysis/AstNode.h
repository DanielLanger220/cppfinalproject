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

    AstNode(AstType type, SourceCodeReference location) : type{type}, location{location} {}

    virtual ~AstNode() = default;

    virtual void accept(AstVisitor& v) = 0;

    SourceCodeReference location;
    const AstType type;

};


class AstArithmeticExpression : public AstNode {

    public:

    enum class Subtype {
        Literal,
        Identifier,
        Unary,
        Binary
    };

    AstArithmeticExpression(Subtype subtype, SourceCodeReference location) : AstNode{AstNode::AstType::AstArithmeticExpression, location}, subtype{subtype} {}


    //virtual std::optional<int64_t> evaluate(const SourceCodeManager& manager, const SymbolTable& table) = 0;

    const Subtype subtype{};

};

class AstLiteral : public AstArithmeticExpression {

    public:

    AstLiteral(SourceCodeReference location, int64_t value) : AstArithmeticExpression{AstArithmeticExpression::Subtype::Literal, location} , value{value} {}

    void accept(AstVisitor& v) override {v.visit(*this);}

    const int64_t value{};

};

class AstIdentifier : public AstArithmeticExpression {

    public:

    AstIdentifier(SourceCodeReference location, size_t index) : AstArithmeticExpression{AstArithmeticExpression::Subtype::Identifier, location} , index{index} {}

    void accept(AstVisitor& v) override {v.visit(*this);}

    const size_t index{};
};


class AstBinaryArithmeticExpression : public AstArithmeticExpression {

    public:

    enum class ArithmeticOperation {
        Plus,
        Minus,
        Mul,
        Div
    };

    AstBinaryArithmeticExpression(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> lhs, std::unique_ptr<AstArithmeticExpression> rhs, ArithmeticOperation op) : AstArithmeticExpression{AstArithmeticExpression::Subtype::Binary, location},
                                                                                                                                                              lhs{std::move(lhs)},
                                                                                                                                                              rhs{std::move(rhs)}, op{op}{}

    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> lhs{};
    std::unique_ptr<AstArithmeticExpression> rhs{};
    const ArithmeticOperation op{};

};


class AstUnaryArithmeticExpression : public AstArithmeticExpression {

    public:

    AstUnaryArithmeticExpression(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> subexpr) : AstArithmeticExpression{AstArithmeticExpression::Subtype::Unary, location},
                                                                                                                   subexpr{std::move(subexpr)} {}

    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> subexpr{};

};


class AstStatement : public AstNode {

    public:

    enum class SubType {
        AstAssignment,
        AstReturn
    };

    AstStatement(SourceCodeReference location, SubType subtype) : AstNode{AstNode::AstType::AstStatement, location} , subtype{subtype} {}

    //virtual void execute(const SourceCodeManager& manager, const SymbolTable& table) = 0;

    const SubType subtype;

};


class AstAssignment : public AstStatement {

    public:


    AstAssignment(SourceCodeReference location, std::unique_ptr<AstIdentifier> lhs, std::unique_ptr<AstArithmeticExpression> rhs) : AstStatement{location, AstStatement::SubType::AstAssignment},
                                                                                                                                    lhs{std::move(lhs)},
                                                                                                                                    rhs{std::move(rhs)} {}

    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstIdentifier> lhs{};
    std::unique_ptr<AstArithmeticExpression> rhs{};

};

class AstReturn : public AstStatement {

    public:

    AstReturn(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> returnvalue) : AstStatement{location, AstStatement::SubType::AstReturn},
                                                                                                    returnvalue{std::move(returnvalue)} {}


    void accept(AstVisitor& v) override {v.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> returnvalue{};
};


class AstFunction {

    public:

    explicit AstFunction(std::vector<std::unique_ptr<AstStatement>> statements) : statements{std::move(statements)} {}


    std::vector<std::unique_ptr<AstStatement>> statements;
};


} // namespace jit

#endif //PLJIT_ASTNODE_H
