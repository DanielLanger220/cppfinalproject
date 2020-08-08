#include "Pljit.h"
#include "Parser/Parser.h"
#include "Parser/ParsePrintVisitor.h"
#include "SemanticAnalysis/SemanticAnalyser.h"
#include "Evaluation/EvalInstance.h"
#include "SemanticAnalysis/DeadCodeOpt.h"
#include "SemanticAnalysis/ConstantPropOpt.h"
#include "SemanticAnalysis/AstPrintVisitor.h"


using namespace std;

namespace jit {

Pljit::PljitHandle Pljit::registerFunction(string sourceCode) {


    veclock.lock();

    PljitHandle handle{this, vecFunctions.size()};
    vecFunctions.emplace_back(FunctionEntry{move(sourceCode)});

    veclock.unlock();


    return handle;
}

unique_ptr<AstFunction> Pljit::compileFunction(size_t index) {

    // Parse the sourcecode

    Parser parser{vecFunctions[index].sourceCode, vecFunctions[index].manager};

    auto parsetree = parser.parseFunction();

    if (!parsetree)
        return nullptr;

    // Do the semantical analysis

    SemanticAnalyser seman{vecFunctions[index].manager, *parsetree};

    if (!seman.createTable())
        return nullptr;

    auto function = seman.analyseFunction();

    if (!function)
        return nullptr;

    // Run the two optimisation passes on the function object

    DeadCodeOpt deadcodeopt{};
    ConstantPropOpt constpropop{};

    function->optimise(deadcodeopt);
    function->optimise(constpropop);


    return function;
}

optional<int64_t> Pljit::PljitHandle::operator()(vector<int64_t> args) {

    if (!functionptr) {     // Handle does not yet have a pointer to the function

        if (jit->vecFunctions[index].compileStatus != 2) {  // Function has not been compiled yet

            // Lock function vector
            jit->veclock.lock();

            // Indicates, if this thread will have to compile the function
            bool compile{false};

            if (jit->vecFunctions[index].compileStatus == 0) {
                jit->vecFunctions[index].compileStatus = 1;
                compile = true;
            }

            // Unlock function vector
            jit->veclock.unlock();

            if (compile) { // CompileStatus was 0 (not compiled yet) and set to 1 (Compiling is in progress) ==> this thread has to compile the function

                    auto function = jit->compileFunction(index);

                    jit->veclock.lock();

                    if (function) {
                        jit->vecFunctions[index].function = move(function);
                        jit->vecFunctions[index].compileStatus = 2;
                    }
                    else {
                        jit->vecFunctions[index].function = nullptr;
                        functionptr = nullptr;
                        jit->vecFunctions[index].compileStatus = 2;
                        return nullopt;
                    }

                    jit->veclock.unlock();

            }
            else {  // Function is either currently compiled by another thread or compiling was finished in the meantime ==> wait until compileStatus is set to 2,
                    // then read the function object

                    while(jit->vecFunctions[index].compileStatus != 2) {
                    }
            }
        }

        // Update the function pointer in the handle (might also be nullptr if compiling of the function failed)
        functionptr = jit->vecFunctions[index].function.get();
    }

    if (!functionptr.value()) {

        cerr << "error: function could not be compiled.\nCompilation aborted\n";

        return nullopt;
    }

    // Function is compiled and correct ==> run the function
    EvalInstance evalInstance{*functionptr.value(), jit->vecFunctions[index].manager};

    return evalInstance.evaluate(move(args));
}


void Pljit::printAst(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (!h.functionptr) {
        cerr << "error: Abstract syntax tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.functionptr.value() == nullptr) {
        cerr << "error: Abstract syntax tree cannot be printed. Function could not be compiled\n";
        return;
    }

    AstPrintVisitor printer{filename, vecFunctions[h.index].manager};
    printer.visit(*vecFunctions[h.index].function);

}

void Pljit::printParseTree(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (!h.functionptr) {
        cerr << "error: Parse tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.functionptr.value() == nullptr) {
        cerr << "error: Parse tree cannot be printed. Function could not be compiled\n";
        return;
    }

    Parser p{vecFunctions[h.index].sourceCode, vecFunctions[h.index].manager};
    auto pt = p.parseFunction();

    assert(pt != nullptr);

    ParsePrintVisitor printer{filename, vecFunctions[h.index].manager};
    printer.printTree(*pt);
}



} // namespace jit