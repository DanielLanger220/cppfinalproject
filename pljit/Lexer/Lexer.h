#ifndef PLJIT_LEXER_H
#define PLJIT_LEXER_H

#include <cctype>
#include <memory>

#include "Token.h"
#include "pljit/CodeManagement/SourceCodeManager.h"

namespace jit {

// Lexer            Performs the lexical analysis of the programming language. Scans the source code and splits it into individual tokens (i.e. terminal symbols of the grammar)
class Lexer {

    public:

    // Constructor
    Lexer(const std::string& sourcecode, const SourceCodeManager& manager) : code{sourcecode}, manager{manager} {
        currAbsPos = code.begin();
    }

    // nextToken                Continues to scan the source code, creates and returns the next token
    std::unique_ptr<Token> nextToken();

    // checkForEndOfFile        Skips all whitespaces starting from the current position. If then the end of the file is reached, returns true. Otherwise returns false
    bool checkForEndOfFile();

    SourceCodeReference refToCurrentPosition() const { return SourceCodeReference{currLine, currPos};}

    private:
    std::string_view code;              // A reference to the source code string
    size_t currLine{1};                 // The current line, where the lexer stands
    size_t currPos{1};                  // The current position within the current line, where the lexer stands
    decltype(code.begin()) currAbsPos;  // An iterator, pointing to the current positon in the source code, where the lexer stands

    const SourceCodeManager& manager;   // A reference to the source code manager


    // Helper methods
    void skipWhitespaces();
};

} // namespace jit

#endif //PLJIT_LEXER_H
