#ifndef PLJIT_EVALINSTANCE_H
#define PLJIT_EVALINSTANCE_H

#include "../SemanticAnalysis/AstNode.h"


namespace jit {

// EvalInstance                         Class used to execute a function given by an Ast with given arguments
class EvalInstance {


    public:

    // Constructor
    EvalInstance(const AstFunction& function, const SourceCodeManager& manager) : function{function}, manager{manager}, identifiers(function.nofidentifiers, 0) {}

    // evaluate             Evaluates the AstFunction object with the given parameters.
    //                      If an error occurs during execution (e.g. division-by-zero), returns nullopt, otherwise returns the result of the function
    std::optional<int64_t> evaluate(std::vector<int64_t> parameters);

    // printErrorMessage    Prints a SourceCodeManager error message (this function is a wrapper and internally just calls the method from the internal SourceCodeManager object)
    void printErrorMessage(const std::string& msg, SourceCodeReference location) const;

    // getValue             Returns the value of the identifier at the given index
    int64_t getValue(size_t index) const {return identifiers[index];}

    // setValue             Sets the value of the identifier at the given index to the given value
    void setValue(size_t index, int64_t value) {identifiers[index] = value;}

    private:

    const AstFunction& function;            // The associated AstFunction object
    const SourceCodeManager& manager;       // Reference to the associated SourceCode Manager
    std::vector<int64_t> identifiers{};     // Tracks the values of the identifiers during execution of the function

};

} // namespace jit

#endif //PLJIT_EVALINSTANCE_H
