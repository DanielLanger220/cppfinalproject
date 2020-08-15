#include "EvalInstance.h"

using namespace std;

namespace jit {

optional<int64_t> EvalInstance::evaluate(std::vector<int64_t> parameters) {


    // Initialise the parameters with the given values

    if (function.nofparameters != parameters.size()) {

        cerr << "error: " << parameters.size() << " parameters given, but function expects " << function.nofparameters << endl;
        return nullopt;
    }

    for (size_t i = 0; i < function.nofparameters; ++i)
        identifiers[i] = parameters[i];


    // Set all variables to 0
    for(size_t i = function.nofparameters; i < function.nofparameters + function.nofvariables; ++i)
        identifiers[i] = 0;


    // Execute the statements of the function in order
    for (auto& s : function.statementlist->statements)
    {
        auto result = s->evaluate(*this);

        if (!result)
            return nullopt;

        if (s->subtype == AstStatement::SubType::AstReturn) {
            res = result;
            return result;
        }
    }

    return 0;
}


void EvalInstance::printErrorMessage(const std::string& msg, SourceCodeReference location) const {

    manager.printErrorMessage(msg, location);
}

} // namespace jit