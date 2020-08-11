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
        bool hasValue{false};               // Indicates whether the variable has a valid value (whether it is initialised)
    };

    // Constructor
    SymbolTable() = default;

    // hasValue         Returns whether the variable at the given index has a valid value
    bool hasValue(size_t index) const;

    // isConst          Returns whether the variable at the given index is a constant
    bool isConst(size_t index) const;


    private:

    // insertEntry      Inserts an object of type TableEntry constructed with the given values at the end of the symbol table
    void insertEntry(SourceCodeReference declaration, bool isConst, bool hasValue);

    std::vector<TableEntry> table{};        // The internal representation of the symbol table
    std::vector<int64_t> constants{};       // Stores the values for the constants
};

} // namespace jit

#endif //PLJIT_SYMBOLTABLE_H
