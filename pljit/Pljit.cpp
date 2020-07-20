#include "Pljit.h"

namespace jit {

int Pljit::registerFunction(std::string sourceCode) {
    vecSourceCode.push_back(std::move(sourceCode));
    return 0;
}

} // namespace jit