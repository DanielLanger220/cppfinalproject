#include "SymbolTable.h"
#include <iostream>

using namespace std;

namespace jit {

void SymbolTable::insertEntry(size_t index, SourceCodeReference declaration, bool isConst, bool hasValue, int64_t value) {

    assert(index == table.size());

    table.emplace_back(TableEntry{declaration, isConst, hasValue, value });
}

int64_t SymbolTable::getValue(size_t index) const {

    assert(index < table.size());

    return table[index].value;
}

void SymbolTable::setValue(size_t index, int64_t value) {

    assert(index < table.size());

    table[index].value = value;
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