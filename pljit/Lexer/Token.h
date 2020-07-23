
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
};

class Identifier : public Token {
    public:

    Identifier(SourceCodeReference loc, std::string_view id) : Token{loc, TokenType::Identifier}, id{id} {}

    std::string_view id;
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
};

} // namespace jit

#endif //PLJIT_TOKEN_H
