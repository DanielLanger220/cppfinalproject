#include <iostream>

#include "Pljit.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "Evaluation/EvalInstance.h"


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
        "Sophie:= Jynx * Aubrey * 8 - 15 + 220;\n"
        "RETURN Sophie\n"
        "END.";


    std::string code1 =
            //"\n\n\n\n\n\n"
            //
            "\n\n\n\n\n\n"
            ""
            ""
            ""
            ""
            "PARAM p, q;\n"
            "VAR v;\n"
            "CONST c = 42, d = 57;\n"
            "BEGIN\n"
            "p := c + d * q;\n"
            "v := 220;\n\n\n\n\n\n"
            "RETURN p * q + c\n"
            "END.";



    Pljit pl{};


    //auto h = pl.registerFunction(code3);

    auto h2 = pl.registerFunction(code1);

    //cout << "Das Ergebnis ist: " << h({220, 284}).value() << endl;

    cout << "Nächstes Ergebnis: " << h2({9,5}).value_or(220) << endl;

    //cout << "Nächstes Ergebnis: " << h({9,5}).value() << endl;

    AstPrintVisitor printvisit{"/home/daniel/parsetree.dot", pl.vecFunctions[0].manager};




    printvisit.visit(*pl.vecFunctions[0].function.value());



}
//---------------------------------------------------------------------------
