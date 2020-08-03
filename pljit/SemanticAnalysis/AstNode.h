#ifndef PLJIT_ASTNODE_H
#define PLJIT_ASTNODE_H

#include <cstdint>
#include <memory>
#include <optional>

#include "AstVisitor.h"
#include "OptimisePass.h"
#include "pljit/SemanticAnalysis/SymbolTable.h"
#include "pljit/SourceCodeManager.h"

namespace jit {

class EvalInstance;

class AstNode {

    public:

    enum class AstType {
        AstFunction,
        AstStatementList,
        AstStatement,
        AstArithmeticExpression,
    };

    // Constructor
    AstNode(SourceCodeReference location, AstType type) : location{location}, type{type} {}

    // Desctructor
    virtual ~AstNode() = default;

    // evaluate                         Evaluates the node according to the values of the given identifiers
    virtual std::optional<int64_t> evaluate(EvalInstance& instance) = 0;


    // accept                           Virtual accept method to support the visitor pattern
    virtual void accept(AstVisitor& v) = 0;

    // optimise                         Virtual method to perform optimisations on the Ast nodes
    virtual void optimise(OptimisePass& opt) = 0;

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

    const Subtype subtype{};        // Specifies the type of the arithmetic expression

};

class AstLiteral : public AstArithmeticExpression {

    public:

    AstLiteral(SourceCodeReference location, int64_t value) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Literal} , value{value} {}

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    const int64_t value{};

};

class AstIdentifier : public AstArithmeticExpression {

    public:

    // Constructor
    AstIdentifier(SourceCodeReference location, size_t index) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Identifier} , index{index} {}

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

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

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    std::unique_ptr<AstArithmeticExpression> lhs{};         // Left hand side expression of this expression
    std::unique_ptr<AstArithmeticExpression> rhs{};         // Right hand side expression of this expression
    const ArithmeticOperation op{};                         // Specifies the arithmetic operation (+, -, *, /) of this expression

};


class AstUnaryArithmeticExpression : public AstArithmeticExpression {

    /*
     * Note: This Expression is always of the form 'expr = - subexpr', i.e. it is always a unary minus expression
     * Unary plus expressions from the parse tree are removed away during the semantic analysis (during the analyse*** methods from the SemanticAnalyser object) as a unary plus
     * has no mathematical effect
     */

    public:

    // Constructor
    AstUnaryArithmeticExpression(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> subexpr) : AstArithmeticExpression{location, AstArithmeticExpression::Subtype::Unary},
                                                                                                                   subexpr{std::move(subexpr)} {}

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

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


    const SubType subtype;          // Specifies the type of the Statement

};


class AstAssignment : public AstStatement {

    public:

    // Constructor
    AstAssignment(SourceCodeReference location, std::unique_ptr<AstIdentifier> lhs, std::unique_ptr<AstArithmeticExpression> rhs) : AstStatement{location, AstStatement::SubType::AstAssignment},
                                                                                                                                    lhs{std::move(lhs)},
                                                                                                                                    rhs{std::move(rhs)} {}
    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    std::unique_ptr<AstIdentifier> lhs{};                       // Pointer to the identifier on the left hand side of the assignment
    std::unique_ptr<AstArithmeticExpression> rhs{};             // Pointer to the expression on the right hand side of the assignment

};

class AstReturn : public AstStatement {

    public:

    // Constructor
    AstReturn(SourceCodeReference location, std::unique_ptr<AstArithmeticExpression> returnvalue) : AstStatement{location, AstStatement::SubType::AstReturn},
                                                                                                    returnvalue{std::move(returnvalue)} {}

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    // optimise


    std::unique_ptr<AstArithmeticExpression> returnvalue{};         // The expression to be returned
};


class AstStatementList : public AstNode {

    public:

    // Constructor
    AstStatementList(SourceCodeReference location, std::vector<std::unique_ptr<AstStatement>> statements) : AstNode{location, AstNode::AstType::AstStatementList} , statements{std::move(statements)} {}

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    // accept           Method to support the visitor pattern
    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    std::vector<std::unique_ptr<AstStatement>> statements;      // Contains the statements of the function

};


class AstFunction : public AstNode {

    public:

    // Constructor
    AstFunction(SourceCodeReference location, std::unique_ptr<AstStatementList> statements, size_t nofidentifiers, size_t nofparameters,
                         size_t nofvariables, std::unique_ptr<std::vector<int64_t>> constants) : AstNode{location, AstType::AstFunction},
                                                                                        statementlist{std::move(statements)},
                                                                                                 nofidentifier{nofidentifiers},
                                                                                                 nofparameters{nofparameters},
                                                                                                 nofvariables{nofvariables},
                                                                                                 constants{std::move(constants)} {}

    std::unique_ptr<AstStatementList> statementlist;               // Contains the statements of the function

    const size_t nofidentifier{};                              // Total numbers of valid identifiers in the function (number of parameters + variables + constants)
    const size_t nofparameters{};
    const size_t nofvariables{};

    std::optional<int64_t> evaluate(EvalInstance& instance) override;

    void accept(AstVisitor& v) override {v.visit(*this);}

    void optimise(OptimisePass& opt) override {opt.visit(*this);}

    std::unique_ptr<std::vector<int64_t>> constants{};

};


} // namespace jit

#endif //PLJIT_ASTNODE_H
