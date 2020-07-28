#include "Token.h"

namespace jit {

std::string Keyword::toString(jit::Keyword::KeywordType t) {

    switch(t) {

        case KeywordType::Begin:
            return "BEGIN";
        case KeywordType::End:
            return "END";
        case KeywordType::Parameter:
            return "PARAM";
        case KeywordType::Var:
            return "VAR";
        case KeywordType::Ret:
            return "RETURN";
        case KeywordType::Constant:
            return "CONST";

    }
}

std::string ArithmeticOperator::toString(ArithmeticOperator::ArithmeticType t) {

    switch(t) {

        case ArithmeticType::Assign:
            return "=";
        case ArithmeticType::VarAssign:
            return ":=";
        case ArithmeticType::Plus:
            return "+";
        case ArithmeticType::Minus:
            return "-";
        case ArithmeticType::Mul:
            return "*";
        case ArithmeticType::Div:
            return "/";
    }

}

std::string Separator::toString(Separator::SeparatorType t) {

    switch(t) {

        case SeparatorType::OpenPar:
            return "(";
        case SeparatorType::ClosePar:
            return ")";
        case SeparatorType::SemiColon:
            return ";";
        case SeparatorType::Comma:
            return ",";
        case SeparatorType::Dot:
            return ".";
    }


}
} // namespace jit
