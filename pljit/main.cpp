#include <iostream>

#include "Pljit.h"

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
        "PARAM a, b;\n"
        "VAR c;\n\n\n"
        "CONST d = 42, e = 13;\n\n\n"
        "BEGIN\n"
        "c := 2;\n"
        "c := 13 + (3 * a) - +(b + (d * e) / a);\n"
        "RETURN c;\n"
        "a:= b * c * 8 - 15 + 220;\n"
        "RETURN a;\n"
        "RETURN 220284;\n"
        "b:= 3;\n"
        "RETURN a\n"
        "END.\n\n\n\n\n";


        std::string code2 =
            "\n\n\n\n\n\n"
            "PARAM p, q;\n"
            "VAR v;\n"
            "CONST c = 42, x = 3, d = 57;\n"
            "BEGIN\n"
            "p := 220;\n"
            "q := 1;\n"
            "v :=  p + 284;\n\n\n\n\n\n"
            "RETURN p * q + v\n"
            "END.";


    Pljit pl{};


    auto h1 = pl.registerFunction(code1);


    thread t1(run, h1, 220, 284);
    thread t2(run, h1, 13, 7);
    thread t3(run, h1, 1, 2);





    //cout << "Das Ergebnis ist: " << h1({220, 284}).value_or(284) << endl;


    auto h2 = pl.registerFunction(code2);

    h2({1,2});

    pl.printAst(h2, "/home/daniel/Ast.dot");



    t1.join();
    t2.join();
    t3.join();

    //this_thread::sleep_for(5s);

    pl.printAst(h1, "/home/daniel/ast220.dot");

    //pl.printParseTree(h1, "/home/daniel/parsetree220.dot");


}
//---------------------------------------------------------------------------
