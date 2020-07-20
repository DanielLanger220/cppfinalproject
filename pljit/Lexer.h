#ifndef PLJIT_LEXER_H
#define PLJIT_LEXER_H

#include <cctype>
#include <memory>

#include "SourceCodeManager.h"
#include "Token.h"

namespace jit {

class Lexer {
    public:
    explicit Lexer(const std::string& sourcecode, const SourceCodeManager& manager) : code{sourcecode}, manager{manager} {
        currAbsPos = code.begin();
    }

    std::unique_ptr<Token> nextToken();

    private:
    std::string_view code;
    size_t currLine{1};
    size_t currPos{1};
    decltype(code.begin()) currAbsPos;

    const SourceCodeManager& manager;
};

} // namespace jit

#endif //PLJIT_LEXER_H
