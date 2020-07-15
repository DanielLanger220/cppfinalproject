
#include "SourceCodeManager.h"

#ifndef PLJIT_TOKEN_H
#define PLJIT_TOKEN_H

class Token {

    public:

    enum class TokenType {
        Keyword,                // 0
        Separator,              // 1
        Identifier,             // 2
        Literal,                // 3
        ArithmeticOperator      // 4
    };

    explicit Token(SourceCodeReference loc, TokenType type) : tokentype{type}, location{loc} {}
    virtual ~Token() = default;
    TokenType tokentype;

    private:
    SourceCodeReference location;
};



class Keyword : public Token {

    public:
    enum class KeywordType{
        Parameter,
        Var,
        Constant,
        Ret,
        Begin,
        End
    };

    Keyword(SourceCodeReference loc, KeywordType type) : Token{loc, TokenType::Keyword}, type{type} {}

    KeywordType type;
};


class Identifier : public Token {

    public:

    Identifier(SourceCodeReference loc, std::string_view id) : Token{loc, TokenType::Identifier}, id{id} {}
    std::string_view id;
};

class Literal : public Token {

    public:

    Literal(SourceCodeReference loc, int64_t val) : Token{loc, TokenType::Literal}, value{val} {}

    int64_t value;

};


class ArithmeticOperator : public Token {

    public:
    enum class ArithmeticType {
        Plus,
        Minus,
        Mul,
        Div,
        VarAssign,   // :=
        Assign       // =
    };

    ArithmeticOperator(SourceCodeReference loc, ArithmeticType type) : Token{loc, TokenType::ArithmeticOperator}, type{type} {}

    public:
    ArithmeticType type;

};

class Separator : public Token {

    public:
    enum class SeparatorType {
        Dot,
        Comma,
        SemiColon,
        OpenPar,
        CloasePar
    };

    Separator(SourceCodeReference loc, SeparatorType type) : Token{loc, TokenType::Separator}, type{type} {}

    SeparatorType type;
};



#endif //PLJIT_TOKEN_H
