#include "Lexer.h"

using namespace std;

std::unique_ptr<Token> Lexer::nextToken() {

    // move lexer position to the beginning of the next token (i.e. skip all whitespaces)
    while(isspace(*currAbsPos)) {
        if (*currAbsPos == '\n') {
            currPos = 0;
            ++currLine;
        }
        else
            ++currPos;

        ++currAbsPos;
    }

    if (currAbsPos == code.end())
        return nullptr;

    // The resulting token
    std::unique_ptr<Token> res;

    // Check for literal
    if (isdigit(*currAbsPos)) {

       int64_t value = strtol(currAbsPos, 0, 10);

        res = make_unique<Literal>(SourceCodeReference(currLine, currPos), value);

        // Move Lexer position to the end of the literal
        while(isdigit(*currAbsPos)) {
            ++currPos;
            ++currAbsPos;
        }

        return res;
    }
    // Check for Keyword and Identifier
    else if (isalpha(*currAbsPos)) {

        // calculate the length of the token
        size_t n = 0;
        while(isalpha(currAbsPos[n]))
            ++n;

        string_view tk(currAbsPos, n);

        if (tk == "PARAM")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::Parameter);
        else if (tk == "VAR")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::Var);
        else if (tk == "CONST")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::Constant);
        else if (tk == "BEGIN")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::Begin);
        else if (tk == "END")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::End);
        else if (tk == "RETURN")
            res = make_unique<Keyword>(SourceCodeReference(currLine, currPos, n), Keyword::KeywordType::Ret);
        else
            res = make_unique<Identifier>(SourceCodeReference(currLine, currPos, n), tk);

        currAbsPos += n;
        currPos += n;
        return res;
    }
    // Check for Separator tokens
    else if (*currAbsPos == '.') {

        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), Separator::SeparatorType::Dot);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == ',') {

        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), Separator::SeparatorType::Comma);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == ';') {

        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), Separator::SeparatorType::SemiColon);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == '(') {

        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), Separator::SeparatorType::OpenPar);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == ')') {

        res = make_unique<Separator>(SourceCodeReference(currLine, currPos), Separator::SeparatorType::CloasePar);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    // Check for Arithmetic operator tokens
    else if (*currAbsPos == '+') {

        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticOperator::ArithmeticType::Plus);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == '-') {

        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticOperator::ArithmeticType::Minus);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == '*') {

        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticOperator::ArithmeticType::Mul);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == '/') {

        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticOperator::ArithmeticType::Div);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == '=') {

        res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos), ArithmeticOperator::ArithmeticType::Assign);
        ++currPos;
        ++currAbsPos;
        return res;
    }
    else if (*currAbsPos == ':') {

        if (*(currAbsPos + 1) == '=') {
            res = make_unique<ArithmeticOperator>(SourceCodeReference(currLine, currPos, 2), ArithmeticOperator::ArithmeticType::VarAssign);
            currPos += 2;
            currAbsPos += 2;
            return res;
        }
        else {
            manager.printErrorMessage("expected '=' after ':'", SourceCodeReference(currLine, currPos, 2));
            return nullptr;
        }
    }

    manager.printErrorMessage("Unrecognized Character", SourceCodeReference{currLine,currPos});

    return nullptr;
}