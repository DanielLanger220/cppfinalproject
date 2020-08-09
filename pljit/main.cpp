#include <iostream>

#include "Pljit.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "Evaluation/EvalInstance.h"
#include "Parser/Parser.h"


#include <mutex>
#include <thread>

//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------


void run(Pljit::PljitHandle handle, int64_t a, int64_t b){

    //if (a == 220)
    //    this_thread::sleep_for(1s);
    auto res = handle({a,b});
    cout << "Resultat: " << res.value_or(220284) << endl;
}

int main() {


        std::string code1 =
        "PARAM Aubrey, Sophie;\n"
        "VAR Steffi;\n\n\n"
        "CONST Jynx = 42, Melissa = 13;\n\n\n"
        "BEGIN\n"
        "Steffi := 2;\n"
        "Steffi := 13 + (3 * Steffi) - +(Jynx + Melissa * Sophie / Aubrey);\n"
        "Sophie:= Jynx * Sophie * 8 - 15 + 220;\n"
        "RETURN Steffi;\n"
        "RETURN 220284;\n"
        "Steffi:= 3;\n"
        "RETURN Steffi\n"
        "END.\n\n\n\n\n";


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


    thread t1(run, h1, 220, 284);
    thread t2(run, h1, 13, 7);
    thread t3(run, h1, 1, 2);



    std::mutex m{};

    cout << sizeof(m) << endl;

    //cout << "Das Ergebnis ist: " << h1({220, 284}).value_or(284) << endl;





    t1.join();
    t2.join();
    t3.join();

    //this_thread::sleep_for(5s);

    pl.printAst(h1, "/home/daniel/ast220.dot");

    pl.printParseTree(h1, "/home/daniel/parsetree220.dot");

    string s1{"Daniel + Sophie!"};


}
//---------------------------------------------------------------------------
