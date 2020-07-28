#include <iostream>

#include "SourceCodeManager.h"
#include "pljit/Lexer/Lexer.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "pljit/Parser/Parser.h"
#include "SemanticAnalysis/SemanticAnalyzer.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include "Pljit.h"

//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------
int main() {

    std::string code3 =
                        "VAR Steffi, Jynx;\n\n\n"
                        "CONST Sophie = 220;\n\n\n"
                        "BEGIN\n"
                        "Jynx := 284;;\n"
                        "Steffi := 3 * 284;\n"
                        "Jynx := (220 + 13)  / -300;\n"
                        "RETURN 284 - (-12)\n"
                        "END.";



    Pljit pl{};

    /*cout << code3 << "Sophie\n";

    cout << "++++++++++++++++\n";
*/
    pl.formatSourceCode(code3);

    //cout << code3 << "Sophie\n";



    Parser p{code3};

    auto n = p.parseFunction();

    if (!n)
        return 1;



    //cout << n->nodes.size() << endl;

    /*for (size_t i = 0; i < n->nodes.size(); ++i)
    {
        cout << i << endl;
        cout << p.manager.getString(n->nodes[i]->location) << endl;
    }*/

    ParsePrintVisitor printer{"/home/daniel/220.dot", p.manager};
    printer.printTree(*n);



    SemanticAnalyzer seman{p.manager, *n};

    if (!seman.createTables())
        return 1;



    std::unique_ptr<AstFunction> func = seman.analyseFunction();

    if (!func)
        return 1;

    AstPrintVisitor astprinter{"/home/daniel/284.dot", p.manager};

    astprinter.printFunction(*func);

}
//---------------------------------------------------------------------------
