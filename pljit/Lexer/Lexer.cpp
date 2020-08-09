#include "Lexer.h"


using namespace std;


namespace jit {

using SeparatorType = Separator::SeparatorType;
using ArithmeticType = ArithmeticOperator::ArithmeticType;
using KeywordType = Keyword::KeywordType;

unique_ptr<Token> Lexer::nextToken() {

    // move lexer position to the beginning of the next token (i.e. skip all whitespaces) and check if end of file is reached
    if (checkForEndOfFile())
    {
        cerr << "error: Unexpected end of file\n";
        return nullptr;
    }

    // The resulting token
    unique_ptr<Token> res{};

    // Check for literal
    if (isdigit(*currAbsPos)) {
        int64_t value = strtol(currAbsPos, nullptr, 10);

        size_t n{0};

        while (isdigit(*(currAbsPos + n)))
            ++n;

        res = make_unique<Literal>(SourceCodeReference(currLine, currPos, n), value);

        // Move Lexer position to the end of the literal
        while (isdigit(*currAbsPos)) {
            ++currPos;
            ++currAbsPos;
        }

        return res;
    }
    // Check for Keyword and Identifier
    else if (isalpha(*currAbsPos)) {
        // calculate the length of the token
        size_t n = 0;
        while (isalpha(currAbsPos[n]))
            ++n;

        string_view tk(currAbsPos, n);

        if (tk == "PARAM")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::Parameter);
        else if (tk == "VAR")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::Var);
        else if (tk == "CONST")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::Constant);
        else if (tk == "BEGIN")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::Begin);
        else if (tk == "END")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::End);
        else if (tk == "RETURN")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), KeywordType::Ret);
        else {
            res = make_unique<Identifier>(SourceCodeReference(currLine, currPos, n));
        }
        currAbsPos += n;
        currPos += n;
        return res;
    }
    // Check for Separator tokens
    else if (*currAbsPos == '.') {
        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), SeparatorType::Dot);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == ',') {
        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), SeparatorType::Comma);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == ';') {
        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), SeparatorType::SemiColon);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == '(') {
        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), SeparatorType::OpenPar);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == ')') {
        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), SeparatorType::ClosePar);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    // Check for Arithmetic operator tokens
    else if (*currAbsPos == '+') {
        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticType::Plus);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == '-') {
        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticType::Minus);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == '*') {
        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticType::Mul);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == '/') {
        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticType::Div);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == '=') {
        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticType::Assign);
        ++currPos;
        ++currAbsPos;
        return res;
    } else if (*currAbsPos == ':') {
        if (currAbsPos < code.end() && *(currAbsPos + 1) == '=') {
            res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos, 2), ArithmeticType::VarAssign);
            currPos += 2;
            currAbsPos += 2;
            return res;
        } else {
            manager.printErrorMessage("expected '=' after ':'", SourceCodeReference(currLine, currPos, 2));
            return nullptr;
        }
    }

    manager.printErrorMessage("Unrecognized Character", SourceCodeReference{currLine, currPos});

    return nullptr;
}

void Lexer::skipWhitespaces() {

    while (currAbsPos != code.end() && isspace(*currAbsPos)) {
        if (*currAbsPos == '\n') {
            currPos = 1;
            ++currLine;
        } else
            ++currPos;

        ++currAbsPos;
    }
}
bool Lexer::checkForEndOfFile() {

    skipWhitespaces();

    return (currAbsPos == code.end());
}

} // namespace jit