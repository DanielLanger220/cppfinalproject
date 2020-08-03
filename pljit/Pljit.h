#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <string>
#include <vector>

namespace jit {

class Pljit {

    // Pljit will be the owner of the source code strings




    public:
    int registerFunction(std::string sourceCode);

    //private:
    std::vector<std::string> vecSourceCode;

    // Helper methods

    // formatSourceCode         Formats the end of the source code so that it has exactly one new-line character at the end
    void formatSourceCode(std::string &sourcecode);
};

} // namespace jit

#endif //PLJIT_PLJIT_H
