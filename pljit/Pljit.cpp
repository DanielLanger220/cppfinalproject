#include "Pljit.h"
#include "Parser/Parser.h"
#include "Parser/ParsePrintVisitor.h"
#include "SemanticAnalysis/SemanticAnalyser.h"
#include "Evaluation/EvalInstance.h"
#include "SemanticAnalysis/DeadCodeOpt.h"
#include "SemanticAnalysis/ConstantPropOpt.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

namespace jit {

Pljit::PljitHandle Pljit::registerFunction(string sourceCode) {

    PljitHandle handle{this, vecFunctions.size()};

    vecFunctions.emplace_back(HandleEntry{move(sourceCode)});
    

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

    if (!handle) {

        if (jit->vecFunctions[index].compileStatus.load() != 2) {
            // Function has not been compiled yet


            unsigned char c{0};
            bool b = jit->vecFunctions[index].compileStatus.compare_exchange_strong(c, 1);

            /*if (args.front() == 220)
                this_thread::sleep_for(5s);
            else if (args.front() == 13)
                this_thread::sleep_for(3s);
            else
                this_thread::sleep_for(1s);
*/

            if (b) { // CompileStatus was 0 (not compiled yet) and set to 1 (Compiling is in progress) ==> this thread has to compile the function

                    cout << "Function gets compiled " << args.front() << endl;


                    auto function = jit->compileFunction(index);

                    if (function) {
                        jit->vecFunctions[index].function = move(function);
                        jit->vecFunctions[index].compileStatus.store(2);
                    }
                    else {
                        jit->vecFunctions[index].function = nullptr;
                        handle = nullptr;
                        jit->vecFunctions[index].compileStatus.store(2);
                        return nullopt;
                    }

            }
            else {  // Function is either currently compiled by another thread or compiling was finished in the meantime ==> wait until compileStatus is set to 2,
                    // then read the function object


                    while(jit->vecFunctions[index].compileStatus.load() != 2) {
                    }

                }
        }

        handle = jit->vecFunctions[index].function.get();
    }

    if (!handle.value()) {

        cerr << "error: function could not be compiled.\nCompilation aborted\n";

        return nullopt;
    }

    EvalInstance evalInstance{*handle.value(), jit->vecFunctions[index].manager};

    return evalInstance.evaluate(move(args));
}


void Pljit::printAst(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (!h.handle) {
        cerr << "error: Abstract syntax tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.handle.value() == nullptr) {
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

    if (!h.handle) {
        cerr << "error: Parse tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.handle.value() == nullptr) {
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