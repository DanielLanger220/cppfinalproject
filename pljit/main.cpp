#include <iostream>

#include "pljit/Pljit/Pljit.h"

//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------

int main() {

    // Example usage:

    string program = "PARAM a, b;\n"
                     "VAR c, d;\n"
                     "BEGIN\n"
                     "c := 2 * a;\n"
                     "d := c + b;\n"
                     "RETURN c * d + a\n"
                     "END.";


    Pljit jit{};

    // register a function
    auto h = jit.registerFunction(program);

    cout << "Evaluation of h(5, 7): " << h({5, 7}).value() << endl;


    // print the parse tree associated with a handle
    //jit.printParseTree(h, "PATH_TO_FIILE");

    // print the (optimised) Ast
    //jit.printAst(h, "PATH_TO_FILE");



}
//---------------------------------------------------------------------------
