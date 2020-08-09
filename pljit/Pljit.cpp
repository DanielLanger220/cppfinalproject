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

Pljit::PljitHandle Pljit::registerFunction(string sourceCode) {


    shared_ptr<FunctionObject> functionobj = make_shared<FunctionObject>(move(sourceCode));

    PljitHandle handle{this, move(functionobj)};

    return handle;
}

unique_ptr<AstFunction> Pljit::compileFunction(FunctionObject& functionobj) {

    // Parse the sourcecode

    Parser parser{functionobj.sourceCode, functionobj.manager};

    auto parsetree = parser.parseFunction();

    if (!parsetree)
        return nullptr;

    // Do the semantical analysis

    SemanticAnalyser seman{functionobj.manager, *parsetree};

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

    shared_lock sl{ptr->m};

    if (!ptr->compileStatus) {

        // Release read access and try to get exclusive access
        sl.unlock();
        unique_lock ul{ptr->m};

        // If function is still not compiled, compile it
        if (!ptr->compileStatus) {

            cout << "function gets compiled " << args.front() << endl;

            auto function = jit->compileFunction(*ptr);
            ptr->function = move(function);
            ptr->compileStatus = true;
        }

        // Release exclusive lock
        ul.unlock();
    }


    // Get read access
    if(!sl.owns_lock())
        sl.lock();


    if (!ptr->function) {

        cerr << "error: Handle belongs to invalid source code\n";
        return nullopt;
    }

    EvalInstance evalInstance{*ptr->function, ptr->manager};
    return evalInstance.evaluate(move(args));
}


void Pljit::printAst(const Pljit::PljitHandle& h, const string& filename) {

    if (this != h.jit) {
        cerr << "error: Handle belongs to a different Pljit object.\n";
        return;
    }

    if (!h.ptr->compileStatus) {
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

    if (!h.ptr->compileStatus) {
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