#ifndef PLJIT_SYMBOLTABLE_H
#define PLJIT_SYMBOLTABLE_H

#include <optional>
#include <cassert>

#include "pljit/Parser/ParseTreeNode.h"
#include "pljit/SourceCodeManager.h"

namespace jit {

class SemanticAnalyser;

class SymbolTable {

    public:

    friend class SemanticAnalyser;

    struct TableEntry {

        TableEntry(SourceCodeReference declaration, bool isConst, bool hasValue) : declaration{declaration}, isConst{isConst}, hasValue{hasValue} {}

        SourceCodeReference declaration;    // Reference to the position in the source code
        bool isConst{false};                // Indicates whether the variable is constant
        bool hasValue{false};               // Indicates whether the variable has a valid value
    };

    // Constructor
    SymbolTable() = default;

    // getValue         Returns the value from the given index
    //int64_t getValue(size_t index) const;

    /*// setValue         Sets the value for the given index
    void setValue(size_t index, int64_t value);
*/
    // hasValue         Returns whether the variable at the given index has a valid value
    bool hasValue(size_t index) const;

    // isConst          Returns whether the variable at the given index is a constant
    bool isConst(size_t index) const;

    //private:

    void insertEntry(size_t index, SourceCodeReference declaration, bool isConst, bool hasValue);

    std::vector<TableEntry> table{};

    std::vector<int64_t> constants{};
};

} // namespace jit

#endif //PLJIT_SYMBOLTABLE_H
