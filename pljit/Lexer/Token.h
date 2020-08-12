
#include "pljit/CodeManagement/SourceCodeManager.h"

#ifndef PLJIT_TOKEN_H
#define PLJIT_TOKEN_H

namespace jit {

// Note: When I wrote this classes a few weeks ago, I decided to put the parentheses '(' and ')' into the category Separator, while they probably rather belong into the
//       category ArithmeticOperator. Looking back, this was maybe not a good decision, but it does not do any harm, so it's hopefully acceptable.


// Token                Base class for all Tokens
class Token {

    public:

    enum class TokenType {
        Keyword,
        Separator,
        Identifier,
        Literal,
        ArithmeticOperator
    };

    // Constructor
    Token(SourceCodeReference loc, TokenType type) : tokentype{type}, location{loc} {}

    const TokenType tokentype;                  // Specifies the type category of the token
    const SourceCodeReference location;         // Reference into the source code of the token

    // Destructor
    virtual ~Token() = default;
};

// Keyword              Token class that represents keywords of the PL
class Keyword : public Token {

    public:

    enum class KeywordType {
        Parameter,
        Var,
        Constant,
        Ret,
        Begin,
        End
    };

    // Constructor
    Keyword(SourceCodeReference loc, KeywordType type) : Token{loc, TokenType::Keyword}, keywordtype{type} {}

    const KeywordType keywordtype;              // Specifies the type of the keyword

    // toString                 Returns a string representation of the given keyword
    static std::string toString(KeywordType t);
};

// Identifier           Token class that represents identifiers of the PL
class Identifier : public Token {

    public:

    // Constructor
    explicit Identifier(SourceCodeReference loc) : Token{loc, TokenType::Identifier} {}
};

// Literal              Token class that represents literal values (64-bit integers)
class Literal : public Token {

    public:

    // Constructor
    Literal(SourceCodeReference loc, int64_t val) : Token{loc, TokenType::Literal}, value{val} {}

    const int64_t value;            // Contains the integer value of the literal
};

// ArithmeticOperator   Token class that represents the operators +, -, *, /, = and :=
class ArithmeticOperator : public Token {

    public:

    enum class ArithmeticType {
        Plus,
        Minus,
        Mul,
        Div,
        VarAssign,  // :=
        Assign      // =
    };

    // Constructor
    ArithmeticOperator(SourceCodeReference loc, ArithmeticType type) : Token{loc, TokenType::ArithmeticOperator}, arithmetictype{type} {}

    const ArithmeticType arithmetictype;        // Specifies the subtype of the arithmetic token

    // toString                 Returns a string representation of the given operator token
    static std::string toString(ArithmeticType t);
};

// Separator            Token class that represents the separators ',', '.', ';', '(', ')'
class Separator : public Token {

    public:

    enum class SeparatorType {
        Dot,
        Comma,
        SemiColon,
        OpenPar,
        ClosePar
    };

    // Constructor
    Separator(SourceCodeReference loc, SeparatorType type) : Token{loc, TokenType::Separator}, separatortype{type} {}

    const SeparatorType separatortype;      // Specifies the subtype of the separator token

    // toString                 Returns a string representation of the given separator token
    static std::string toString(SeparatorType t);
};

} // namespace jit

#endif //PLJIT_TOKEN_H
