#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <mutex>

#include "SourceCodeManager.h"
//#include "SemanticAnalysis/AstNode.h"

namespace jit {

class AstFunction;

class Pljit {

    public:

    class PljitHandle {

        friend class Pljit;

        public:

        // Constructor
        explicit PljitHandle(Pljit* jit, size_t index) : index{index}, jit{jit} {};

        // ()-operator              calls (and perhaps previously compiles) the function associated with the handle. The arguments to the function are given in a vector
        std::optional<int64_t> operator()(std::vector<int64_t> args);

        private:

        // Pointer to the Ast-function object.
        // nullopt ==> Function not yet compiled
        // nullptr ==> Function has been compiled but an error occured
        std::optional<AstFunction*> functionptr{std::nullopt};

        const size_t index;         // Index of the associated function in the function vector
        Pljit* const jit;           // Pointer to the associated Pljit object
    };


    // Constructor
    Pljit() = default;


    // registerFunction         registers the given source code and returns a handle to the function
    PljitHandle registerFunction(std::string sourceCode);

    // printAst                 Prints the abstract syntax tree referenced to by the given handle to the given filename in *.dot format
    void printAst(const PljitHandle& handle, const std::string& filename);

    // printParseTree           Prints the parse tree referenced to by the given handle to the given filename in *.dot format
    //                          NOTE:
    //                          As the parse tree object is not permanently stored by the Pljit object, this needs the source code to be parsed again on the fly to temporary create
    //                          a parse tree object which then is printed.
    //                          As this method is not officially requested and rather for test purposes, I decided to do it that way
    void printParseTree(const PljitHandle& h, const std::string& filename);

    private:

    struct FunctionEntry {

        // Constructor
        explicit FunctionEntry(std::string code) : sourceCode(std::move(code)), manager{sourceCode}  {}

        std::string sourceCode;                         // Source code
        SourceCodeManager manager;                      // Source Code Manager
        unsigned char compileStatus{0};                 // 0 ==> not yet compiled   1 ==> function is currently compiled by a thread    2 ==> Compilation is finished
        std::unique_ptr<AstFunction> function{nullptr}; // Pointer to the Ast-Function object
    };

    // compileFunction          Compiles the function corresponding to the source code at the given index and returns a pointer to an AstFunction object
    std::unique_ptr<AstFunction> compileFunction(size_t index);

    std::vector<FunctionEntry> vecFunctions{};        // Vector that stores the Function entries

    std::mutex veclock{};       // Mutex to ensure thread-safe write access to the vector vecFunctions

};

} // namespace jit

#endif //PLJIT_PLJIT_H
