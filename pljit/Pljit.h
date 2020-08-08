#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <atomic>

#include "SourceCodeManager.h"

#include "SemanticAnalysis/AstNode.h"

namespace jit {

//class AstFunction;

class Pljit {

    public:

    class PljitHandle {

        friend class Pljit;

        public:

        explicit PljitHandle(Pljit* jit, size_t index) : index{index}, jit{jit} {};
        std::optional<int64_t> operator()(std::vector<int64_t> args);

        private:

        std::optional<AstFunction*> handle{std::nullopt};
        const size_t index;
        Pljit* const jit;
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

    struct HandleEntry {

        explicit HandleEntry(std::string code) : sourceCode(std::move(code)), manager{sourceCode}  {}
        HandleEntry(HandleEntry&& m) noexcept : sourceCode{std::move(m.sourceCode)},
                                                manager{std::move(m.manager)},
                                                function{std::move(m.function)},
                                                compileStatus{m.compileStatus.load()} {}

        std::string sourceCode;
        SourceCodeManager manager;
        std::unique_ptr<AstFunction> function{nullptr};
        std::atomic<unsigned char> compileStatus{0};

    };

    // compileFunction          Compiles the function corresponding to the source code at the given index and returns a pointer to an AstFunction object
    std::unique_ptr<AstFunction> compileFunction(size_t index);

    std::vector<HandleEntry> vecFunctions{};


};

} // namespace jit

#endif //PLJIT_PLJIT_H
