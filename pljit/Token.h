
#include "SourceCodeManager.h"

#ifndef PLJIT_TOKEN_H
#define PLJIT_TOKEN_H

namespace jit {


enum class TokenType {
    Keyword,            // 0
    Separator,          // 1
    Identifier,         // 2
    Literal,            // 3
    ArithmeticOperator  // 4
};

enum class KeywordType {
    Parameter,
    Var,
    Constant,
    Ret,
    Begin,
    End
};

enum class SeparatorType {
    Dot,
    Comma,
    SemiColon,
    OpenPar,
    ClosePar
};

enum class ArithmeticType {
    Plus,
    Minus,
    Mul,
    Div,
    VarAssign,  // :=
    Assign      // =
};

class Token {

    public:

    explicit Token(SourceCodeReference loc, TokenType type) : tokentype{type}, location{loc} {}

    const TokenType tokentype;
    const SourceCodeReference location;

    virtual ~Token() = default;
};

class Keyword : public Token {

    public:



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

    int64_t value;
};

class ArithmeticOperator : public Token {

    public:



    ArithmeticOperator(SourceCodeReference loc, ArithmeticType type) : Token{loc, TokenType::ArithmeticOperator}, arithmetictype{type} {}

    const ArithmeticType arithmetictype;
};

class Separator : public Token {

    public:

    Separator(SourceCodeReference loc, SeparatorType type) : Token{loc, TokenType::Separator}, separatortype{type} {}

    SeparatorType separatortype;
};

} // namespace jit

#endif //PLJIT_TOKEN_H
