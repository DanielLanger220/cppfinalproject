#include "SymbolTable.h"
#include <iostream>

using namespace std;

namespace jit {

void SymbolTable::insertEntry(SourceCodeReference declaration, bool isConst, bool hasValue) {

    table.emplace_back(TableEntry{declaration, isConst, hasValue});
}

bool SymbolTable::hasValue(size_t index) const {

    assert(index < table.size());

    return table[index].hasValue;

}

bool SymbolTable::isConst(size_t index) const {

    assert(index < table.size());

    return table[index].isConst;
}

} // namespace jit