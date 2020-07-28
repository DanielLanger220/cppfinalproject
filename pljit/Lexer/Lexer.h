#ifndef PLJIT_LEXER_H
#define PLJIT_LEXER_H

#include <cctype>
#include <memory>
#include <map>

#include "Token.h"
#include "pljit/SourceCodeManager.h"

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
    std::map<std::string_view, size_t> nametable{};
    size_t nofidentifiers{0};
};

} // namespace jit

#endif //PLJIT_LEXER_H
