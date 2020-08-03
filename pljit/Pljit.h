#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace jit {

class AstFunction;

class Pljit {

    public:

    class PljitHandle {

        public:

        explicit PljitHandle(Pljit* jit, size_t index) : index{index}, jit{jit} {};
        std::optional<int64_t> operator()(std::vector<int64_t> args);

        private:

        std::optional<AstFunction*> handle;
        size_t index;
        Pljit* jit;
    };


    // Constructor
    Pljit() = default;


    // registerFunction         registers the given source code and returns a handle to the function
    PljitHandle registerFunction(std::string sourceCode);

    private:

    struct CodeHandlePair {
        explicit CodeHandlePair(std::string sourceCode) : sourceCode(std::move(sourceCode)) {}
        std::string sourceCode;
        std::optional<std::unique_ptr<AstFunction>> function{std::nullopt};
    };

    // compileFunction          Compiles the function corresponding to the source code at the given index and returns a pointer to an AstFunction object
    std::unique_ptr<AstFunction> compileFunction(size_t index);


    std::vector<CodeHandlePair> vecFunctions{};

    // formatSourceCode         Formats the end of the source code so that it has exactly one new-line character at the end
    void formatSourceCode(std::string &sourcecode);
};

} // namespace jit

#endif //PLJIT_PLJIT_H
