#include "Pljit.h"
#include "Parser/Parser.h"
#include "Parser/ParsePrintVisitor.h"
#include "SemanticAnalysis/SemanticAnalyser.h"
#include "Evaluation/EvalInstance.h"
#include "SemanticAnalysis/DeadCodeOpt.h"
#include "SemanticAnalysis/ConstantPropOpt.h"
#include "SemanticAnalysis/AstPrintVisitor.h"

#include <thread>

using namespace std;

namespace jit {


Pljit::Pljit() = default;

Pljit::~Pljit() = default;

Pljit::FunctionObject::FunctionObject(std::string code) : sourceCode(std::move(code)), manager{sourceCode}{
}


Pljit::PljitHandle Pljit::registerFunction(string sourceCode) {

    if (sourceCode.back() != '\n')
        sourceCode.push_back('\n');

    unique_ptr<FunctionObject> functionobj = make_unique<FunctionObject>(move(sourceCode));

    PljitHandle handle{this, functionobj.get()};

    vecfunctions.emplace_back(move(functionobj));

    return handle;
}

unique_ptr<AstFunction> Pljit::compileFunction(const FunctionObject& functionobj) {

    // Parse the sourcecode

    Parser parser{functionobj.sourceCode, functionobj.manager};

    auto parsetree = parser.parseFunction();

    if (!parsetree)
        return nullptr;

    // Do the semantical analysis

    SemanticAnalyser seman{functionobj.manager, *parsetree};

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

    // Check, if the function has not yet been compiled
    if (ptr->compileStatus.load() == 0) {

        unsigned char c = 0;
        bool b = ptr->compileStatus.compare_exchange_strong(c, 1);

        if (b) { // This thread successfully compare-and-swaped the compile-status-flag from 0 to 1 --> this thread has to compile the function

            cout << "function gets compiled " << args.front() << endl;

            auto function = jit->compileFunction(*ptr);
            ptr->function = move(function);
            ptr->compileStatus.store(2);        // Set the compile-status-flag to 2 to signal all other threads that the function is ready
        }
    }

    // Wait until the compile-status flag gets set to 2 (exactly one thread will ensure that this definitely happens)
    while(ptr->compileStatus.load() != 2) {
    }

    // If the pointer now still is a null-pointer this means an error occurred during compiling
    if (!ptr->function) {

        cerr << "error: Handle belongs to invalid source code\n";
        return nullopt;
    }

    // Finally evaluate the function with the given arguments and return the result
    EvalInstance evalInstance{*ptr->function, ptr->manager};
    return evalInstance.evaluate(move(args));
}


void Pljit::printAst(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (h.ptr->compileStatus.load() != 2) {
        cerr << "error: Abstract syntax tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.ptr->function == nullptr) {
        cerr << "error: Abstract syntax tree cannot be printed. Invalid source code.\n";
        return;
    }

    AstPrintVisitor printer{filename, h.ptr->manager};
    printer.visit(*h.ptr->function);

}

void Pljit::printParseTree(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (h.ptr->compileStatus.load() != 2) {
        cerr << "error: Parse tree cannot be printed. Function has not been compiled yet.\n";
        return;
    }

    if (h.ptr->function == nullptr) {
        cerr << "error: Parse tree cannot be printed. Invalid source code.\n";
        return;
    }

    Parser p{h.ptr->sourceCode, h.ptr->manager};
    auto pt = p.parseFunction();

    assert(pt != nullptr);

    ParsePrintVisitor printer{filename, h.ptr->manager};
    printer.printTree(*pt);
}



} // namespace jit