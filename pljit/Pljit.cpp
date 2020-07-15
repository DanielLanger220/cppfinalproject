#include "Pljit.h"

int Pljit::registerFunction(std::string sourceCode) {

    vecSourceCode.push_back(std::move(sourceCode));

}