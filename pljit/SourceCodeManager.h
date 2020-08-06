#ifndef PLJIT_SOURCECODEMANAGER_H
#define PLJIT_SOURCECODEMANAGER_H

#include <string>
#include <vector>
#include <iostream>

namespace jit {


struct SourceCodeReference {

    // Constructor              Initialises a Reference to a position in a source code string determined by line number, position within that line and the length (in characters) of
    //                          the reference
    SourceCodeReference(size_t line, size_t position, size_t range = 1) : line{line}, position{position}, range{range} {}

    // Constructor              Initialises a Reference with the start position of a given reference and a length
    SourceCodeReference(SourceCodeReference c, size_t range) : line{c.line}, position{c.position}, range{range} {}

    const size_t line;          // The line in the source code, the reference refers to
    const size_t position;      // The relative position in the line, the reference refers to
    const size_t range;         // The number of characters the reference covers starting with the position defined by the parameters 'line' and 'position'
};

class SourceCodeManager {

    public:

    // Constructor
    explicit SourceCodeManager(const std::string& sourceCode);

    // printErrorMessage        Prints a given message in the context of a given source code reference
    void printErrorMessage(const std::string& message, const SourceCodeReference& location) const;

    // getString                Returns a string view object belonging to the given source code reference
    std::string_view getString(const SourceCodeReference& loc) const;

    // getabsolutePosition      Returns the absolute position in the source code string for a given line and position in that line
    size_t getabsolutePosition(const SourceCodeReference& ref) const { return lines[ref.line - 1] + ref.position - 1; }

    private:

    std::string_view code;              // A reference to the source code string

    size_t noflines{0};                 // Number of lines of the managed source code string
    std::vector<size_t> lines{};        // A vector storing the absolute positions of the start points of each line in the source code
};

} // namespace jit

#endif //PLJIT_SOURCECODEMANAGER_H
