#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <string>
#include <vector>

namespace jit {

class Pljit {

    public:
    int registerFunction(std::string sourceCode);

    //private:
    std::vector<std::string> vecSourceCode;

    // Helper methods

    // formatSourceCode         Formats the end of the source code so that it has exactly one new-line character at the end
    void formatSourceCode();
};

} // namespace jit

#endif //PLJIT_PLJIT_H
