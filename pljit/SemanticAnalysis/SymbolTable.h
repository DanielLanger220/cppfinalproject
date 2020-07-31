#ifndef PLJIT_SYMBOLTABLE_H
#define PLJIT_SYMBOLTABLE_H

#include <optional>
#include <cassert>

#include "pljit/Parser/ParseTreeNode.h"
#include "pljit/SourceCodeManager.h"

namespace jit {

class SemanticAnalyzer;

class SymbolTable {

    public:

    friend class SemanticAnalyzer;

    struct TableEntry {

        TableEntry(SourceCodeReference declaration, bool isConst, bool hasValue, int64_t value = 0) : declaration{declaration}, isConst{isConst}, hasValue{hasValue}, value{value}{}

        SourceCodeReference declaration;    // Reference to the position in the source code
        bool isConst{false};                // Indicates whether the variable is constant
        bool hasValue{false};               // Indicates whether the variable has a valid value
        int64_t value{0};                   // The value stored in the entry
    };

    // Constructor
    SymbolTable() = default;

    // getValue         Returns the value from the given index
    int64_t getValue(size_t index) const;

    // setValue         Sets the value for the given index
    void setValue(size_t index, int64_t value);

    // hasValue         Returns whether the variable at the given index has a valid value
    bool hasValue(size_t index) const;

    // isConst          Returns whether the variable at the given index is a constant
    bool isConst(size_t index) const;

    //private:

    void insertEntry(size_t index, SourceCodeReference declaration, bool isConst, bool hasValue, int64_t value = 0);

    std::vector<TableEntry> table{};
};

} // namespace jit

#endif //PLJIT_SYMBOLTABLE_H
