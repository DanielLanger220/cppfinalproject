#include "Pljit.h"

using namespace std;

namespace jit {

int Pljit::registerFunction(string sourceCode) {
    vecSourceCode.push_back(move(sourceCode));
    return 0;
}


void Pljit::formatSourceCode(string& sourcecode) {

    // Remove leading and trailing newlines and add exactly one newline at the end of the code
    char newline = '\n';
    size_t p = sourcecode.find_last_not_of(newline);
    sourcecode.erase(p + 1);

    p = sourcecode.find_first_not_of(newline);
    sourcecode.erase(0, p);

    sourcecode.push_back(newline);
}

} // namespace jit