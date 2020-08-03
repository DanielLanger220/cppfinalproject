#ifndef PLJIT_EVALINSTANCE_H
#define PLJIT_EVALINSTANCE_H

#include "../SemanticAnalysis/AstNode.h"


namespace jit {

class EvalInstance {


    public:


    EvalInstance(const AstFunction& function, SourceCodeManager manager) : function{function}, manager{std::move(manager)}, identifiers(function.nofidentifier, 0) {

        // Initialise constants
        for (size_t i = 0; i < function.constants->size(); ++i)
            identifiers[function.nofparameters + function.nofvariables + i] = (*function.constants)[i];

    }

    std::optional<int64_t> evaluate(std::vector<int64_t> parameters);

    int64_t getValue(size_t index) const {return identifiers[index];}
    void setValue(size_t index, int64_t value) {identifiers[index] = value;}

    //private:

    const AstFunction& function;
    const SourceCodeManager manager;
    std::vector<int64_t> identifiers{};

};

} // namespace jit

#endif //PLJIT_EVALINSTANCE_H
