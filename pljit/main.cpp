#include <iostream>

#include "Pljit.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include "SemanticAnalysis/SemanticAnalyser.h"
#include "pljit/Lexer/Lexer.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "pljit/Parser/Parser.h"
#include "Evaluation/EvalInstance.h"
#include "SemanticAnalysis/DeadCodeOpt.h"
#include "SemanticAnalysis/ConstantPropOpt.h"

//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------
int main() {

    /*std::string code3 =
                        "PARAM Aubrey, Sophie;\n"
                        "VAR Steffi;\n\n\n"
                        "CONST Jynx = 3141, Melissa = 2;\n\n\n"
                        "BEGIN\n"
                        "Steffi := 284 / Melissa;\n"
                        "RETURN 284;\n"
                        "Steffi := 3 * 284 / Steffi + Jynx;\n"
                        "Steffi := (220 - (13 * 12) ) / -Sophie + Aubrey;\n"
                        "RETURN Aubrey * Sophie\n"
                        "END.";
*/

        std::string code3 =
        "PARAM Aubrey, Steffi;\n"
        "VAR Sophie;\n\n\n"
        "CONST Jynx = 42, Melissa = 13;\n\n\n"
        "BEGIN\n"
        "Sophie:= Jynx * Aubrey + 220;\n"
        "RETURN Sophie\n"
        "END.";


    std::string code1 =
        "PARAM Aubrey, Steffi;\n"
        "VAR Sophie;\n\n\n"
        "CONST Jynx = 42, Melissa = 57;\n\n\n"
        "BEGIN\n"
        "RETURN Aubrey * Steffi + Jynx\n"
        "END.";



    Pljit pl{};


    auto h = pl.registerFunction(code3);

    auto h2 = pl.registerFunction(code1);

    cout << "Das Ergebnis ist: " << h({220, 284}).value() << endl;

    cout << "Nächstes Ergebnis: " << h2({9,5}).value() << endl;

    cout << "Nächstes Ergebnis: " << h({9,5}).value() << endl;



/*
    ParsePrintVisitor printer{"/home/daniel/220ParseTree.dot", p.manager};
    printer.printTree(*n);

    SemanticAnalyser seman{p.manager, *n};

    if (!seman.createTable())
        return 1;



    std::unique_ptr<AstFunction> func = seman.analyseFunction();

    if (!func)
        return 1;


    DeadCodeOpt dcopt{};
    ConstantPropOpt constopt{};

    //func->optimise(dcopt);
    //func->optimise(constopt);



    AstPrintVisitor astprinter{"/home/daniel/284AST.dot", p.manager};

    func->accept(astprinter);


    EvalInstance ev{*func, p.manager};

    optional<int64_t> result;



    auto eval2 = ev;


    result = eval2.evaluate({9, 5});

    if (!result)
        return 0;

    cout << "Das Ergebnis ist:\t" << result.value() << endl;*/


}
//---------------------------------------------------------------------------
