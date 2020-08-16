#ifndef PLJIT_FUNCTIONOBJECT_H
#define PLJIT_FUNCTIONOBJECT_H

#include <atomic>
#include <memory>

#include "pljit/CodeManagement/SourceCodeManager.h"


namespace jit {


class AstFunction;

// FunctionObject           Wraps all data (source code, source code manager, AstFunction object ...) for a registered function
struct FunctionObject {

    // Constructor
    explicit FunctionObject(std::string code);

    const std::string sourceCode;                       // Source code
    const SourceCodeManager manager;                    // Source Code Manager
    std::atomic<unsigned char> compileStatus{0};        // 0 --> Function not yet compiled  1 --> Function currently gets compiled by one thread   2 --> Compiling finished
    std::unique_ptr<AstFunction> function{nullptr};     // Pointer to the Ast-Function object
};


} // namespace jit

#endif //PLJIT_FUNCTIONOBJECT_H
