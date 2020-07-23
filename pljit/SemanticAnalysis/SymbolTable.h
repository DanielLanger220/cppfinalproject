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

        SourceCodeReference declaration;
        bool isConst{false};
        bool hasValue{false};
        int64_t value{0};
    };

    // Constructor
    SymbolTable() = default;


    int64_t getValue(size_t index) const;
    void setValue(size_t index, int64_t value);
    bool hasValue(size_t index) const;
    bool isConst(size_t index) const;

    //private:

    void insertEntry(size_t index, SourceCodeReference declaration, bool isConst, bool hasValue, int64_t value = 0);

    std::vector<TableEntry> table{};



};

} // namespace jit

#endif //PLJIT_SYMBOLTABLE_H
