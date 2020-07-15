#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <string>
#include <vector>

class Pljit {



    public:

    int registerFunction(std::string sourceCode);



    //private:
    std::vector<std::string> vecSourceCode;


};

#endif //PLJIT_PLJIT_H
