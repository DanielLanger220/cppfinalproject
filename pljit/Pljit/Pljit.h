#ifndef PLJIT_PLJIT_H
#define PLJIT_PLJIT_H

#include <memory>
#include <optional>
#include <vector>


namespace jit {

class AstFunction;
struct FunctionObject;

// Pljit                Creates handles for registered functions and manages the underlying data
class Pljit {

    public:

    // PljitHandle      Represents a handle to a registered functions that can be used to call the execute the function
    class PljitHandle {

        friend class Pljit;

        public:

        // Constructor
        explicit PljitHandle(Pljit* jit, FunctionObject* ptr) : ptr{ptr}, jit{jit} {};

        // ()-operator              calls (and perhaps previously compiles) the function associated with the handle. The arguments to the function are given in a vector
        std::optional<int64_t> operator()(std::vector<int64_t> args);

        private:

        FunctionObject* const ptr;              // Pointer to the associated Function object.
        Pljit* const jit;                       // Pointer to the associated Pljit object
    };


    // Constructor
    Pljit();

    // Destructor
    ~Pljit();

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

    // compileFunction          Compiles the function corresponding to the source code of the function object and returns a pointer to an AstFunction object
    static std::unique_ptr<AstFunction> compileFunction(const FunctionObject& functionobj);

    std::vector<std::unique_ptr<FunctionObject>> vecfunctions{};        // Stores the associated data (source code, source code manager ...) for the registered functions.

};

} // namespace jit

#endif //PLJIT_PLJIT_H
