#include <iostream>

#include "Pljit.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "Evaluation/EvalInstance.h"
#include "Parser/Parser.h"

//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------
int main() {


        std::string code1 =
        "PARAM Aubrey, Sophie;\n"
        "VAR Steffi;\n\n\n"
        "CONST Jynx = 42, Melissa = 13;\n\n\n"
        "BEGIN\n"
        "RETURN 1;\n"
        "Steffi := 2;\n"
        "Steffi := 13 + (3 * Steffi) - +(Jynx + Melissa * Sophie / Aubrey);\n"
        "Sophie:= Jynx * Sophie * 8 - 15 + 220;\n"
        "RETURN Steffi;\n"
        "RETURN 220284;\n"
        "Steffi:= 3;\n"
        "RETURN Steffi\n"
        "END.";


        std::string code2 =
            "\n\n\n\n\n\n"
            "PARAM p, q;\n"
            "VAR v;\n"
            "CONST c = 42, x = 3, d = 57;\n"
            "BEGIN\n"
            "p := c + (-x) * d * q;\n"
            "v := 220;\n\n\n\n\n\n"
            "RETURN p * q + c\n"
            "END.";


    Pljit pl{};


    auto h1 = pl.registerFunction(code1);

    auto h2 = pl.registerFunction(code2);

    cout << "Das Ergebnis ist: " << h1({220, 284}).value_or(284) << endl;

    cout << "Nächstes Ergebnis: " << h2({9,5}).value_or(220) << endl;

    cout << "Nächstes Ergebnis: " << h1({9,5}).value_or(1) << endl;


    pl.printAst(h1, "/home/daniel/ast220.dot");

    pl.printParseTree(h1, "/home/daniel/parsetree220.dot");



}
//---------------------------------------------------------------------------
