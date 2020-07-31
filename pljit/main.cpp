#include <iostream>

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
                        "CONST Sophie = 220, Daniel = 284;\n\n\n"
                        "BEGIN\n"
                        "RETURN 284;\n"
                        "Jynx := 3 * 284;\n"
                        "Jynx := (220 - (13 * 12) ) / -Sophie + Daniel;\n"
                        "RETURN 284 - (-12)\n"
                        "END.";



    Pljit pl{};

    pl.formatSourceCode(code3);



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

    ParsePrintVisitor printer{"/home/daniel/220ParseTree.dot", p.manager};
    printer.printTree(*n);



    SemanticAnalyzer seman{p.manager, *n};

    if (!seman.createTables())
        return 1;



    std::unique_ptr<AstFunction> func = seman.analyseFunction();

    if (!func)
        return 1;

    AstPrintVisitor astprinter{"/home/daniel/284AST.dot", p.manager};

    astprinter.printFunction(*func);

}
//---------------------------------------------------------------------------
