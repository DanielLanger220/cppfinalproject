
#include "../SourceCodeManager.h"

#ifndef PLJIT_TOKEN_H
#define PLJIT_TOKEN_H

namespace jit {

class Token {

    public:

    enum class TokenType {
        Keyword,
        Separator,
        Identifier,
        Literal,
        ArithmeticOperator
    };

    Token(SourceCodeReference loc, TokenType type) : tokentype{type}, location{loc} {}

    const TokenType tokentype;
    const SourceCodeReference location;

    virtual ~Token() = default;
};

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

    const KeywordType keywordtype; // Specifies the keyword

    static std::string toString(KeywordType t);
};

class Identifier : public Token {
    public:

    Identifier(SourceCodeReference loc, size_t index) : Token{loc, TokenType::Identifier}, index{index} {}

    const size_t index;
};

class Literal : public Token {

    public:

    Literal(SourceCodeReference loc, int64_t val) : Token{loc, TokenType::Literal}, value{val} {}

    const int64_t value;
};

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


    ArithmeticOperator(SourceCodeReference loc, ArithmeticType type) : Token{loc, TokenType::ArithmeticOperator}, arithmetictype{type} {}

    const ArithmeticType arithmetictype;

    static std::string toString(ArithmeticType t);
};

class Separator : public Token {

    public:

    enum class SeparatorType {
        Dot,
        Comma,
        SemiColon,
        OpenPar,
        ClosePar
    };

    Separator(SourceCodeReference loc, SeparatorType type) : Token{loc, TokenType::Separator}, separatortype{type} {}

    const SeparatorType separatortype;

    static std::string toString(SeparatorType t);
};

} // namespace jit

#endif //PLJIT_TOKEN_H
