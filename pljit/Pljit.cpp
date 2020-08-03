#include "Pljit.h"
#include "Parser/Parser.h"
#include "SemanticAnalysis/SemanticAnalyser.h"
#include "Evaluation/EvalInstance.h"

using namespace std;

namespace jit {

Pljit::PljitHandle Pljit::registerFunction(string sourceCode) {

    PljitHandle handle{this, vecFunctions.size()};

    vecFunctions.emplace_back(CodeHandlePair{move(sourceCode)});

    return handle;
}


void Pljit::formatSourceCode(string& sourcecode) {

    // Remove leading and trailing newlines and add exactly one newline at the end of the code
    char newline = '\n';
    size_t p = sourcecode.find_last_not_of(newline);
    sourcecode.erase(p + 1);

    p = sourcecode.find_first_not_of(newline);
    sourcecode.erase(0, p);

    sourcecode.push_back(newline);
}

std::unique_ptr<AstFunction> Pljit::compileFunction(size_t index) {

    SourceCodeManager manager{vecFunctions[index].sourceCode};

    Parser parser{vecFunctions[index].sourceCode};

    auto parsetree = parser.parseFunction();

    if (!parsetree)
        return nullptr;

    SemanticAnalyser seman{manager, *parsetree};

    if (!seman.createTable())
        return nullptr;

    auto function = seman.analyseFunction();

    return function;
}

optional<int64_t> Pljit::PljitHandle::operator()(std::vector<int64_t> args) {

    if (!handle) {

        if (!jit->vecFunctions[index].function) {

            auto function = jit->compileFunction(index);

            if (function)
                jit->vecFunctions[index].function = move(function);
            else
                return nullopt;
        }

        handle = jit->vecFunctions[index].function->get();
    }

    if (!handle)
        return nullopt;

    SourceCodeManager manager{jit->vecFunctions[index].sourceCode};

    EvalInstance evalInstance{*handle.value(), manager};


    return evalInstance.evaluate(move(args));
}

} // namespace jit