#ifndef PLJIT_SOURCECODEMANAGER_H
#define PLJIT_SOURCECODEMANAGER_H


#include <string>
#include <vector>
#include <iostream>

namespace jit {

struct SourceCodeReference {

    SourceCodeReference(size_t line, size_t position, size_t range = 1) : line{line}, position{position}, range{range} {}

    SourceCodeReference(SourceCodeReference c, size_t range) : line{c.line}, position{c.position}, range{range} {}

    const size_t line;
    const size_t position;
    const size_t range;
};

class SourceCodeManager {

    public:

    // Constructor
    explicit SourceCodeManager(std::string& sourceCode);

    // printErrorMessage        Prints a given message in the context of a given source code reference
    void printErrorMessage(const std::string& message, const SourceCodeReference& location) const;

    // getString                Returns a string view object belonging to the given source code reference
    std::string_view getString(const SourceCodeReference& loc) const;

    // getabsolutePosition      Returns the absolute position in the source code string for a given line and position in that line
    size_t getabsolutePosition(const SourceCodeReference& ref) const { return lines[ref.line - 1] + ref.position - 1; }

    private:
    std::string& code; //TODO: make code constant
    size_t noflines{0};
    std::vector<size_t> lines{};
};

} // namespace jit

#endif //PLJIT_SOURCECODEMANAGER_H
