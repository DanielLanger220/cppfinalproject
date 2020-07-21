#include <iostream>

#include "SourceCodeManager.h"
#include "Lexer.h"
#include "Parser.h"
#include "ParsePrintVisitor.h"
//---------------------------------------------------------------------------
using namespace std;
using namespace jit;
//---------------------------------------------------------------------------
int main() {

    std::string code = "Daniel liebt die niedliche :=Sophie BEGIN, 220 + 284 - 97 * 22028= 504\n(220 + RETURN220 = 440);";


    SourceCodeManager manager(code);



    Lexer lex(code, manager);

    std::unique_ptr<Token> res;

//    std::string code3 = "(220 * 13) * 57 + 284\n";

    //std::string code3 = "BEGIN Sophie := Daniel;\nSophie := 220 * -Daniel * (57 + 284);\nRETURN 3;Sophie := 284\nEND\n";

    std::string code3 = "PARAM Sophie; CONST Daniel = 284; BEGIN Sophie := Daniel;\nSophie := 220 + 284 ;\nRETURN 284  END.\n";

    std::string code4 = "CONST Sophie = 220, Daniel = 284, Sophie = 95, Jynx = 83, Kata = 41;\n";

    Parser p{code3};

    auto n = p.parseFunction();

    cout << n->nodes.size() << endl;

    for (size_t i = 0; i < n->nodes.size(); ++i)
    {
        if (i == 0)
        {
            //for(auto &n2 : static_cast<MultExprNode*>(n->nodes[i].get())->nodes)
            //    cout << "\t\t" << p.manager.getString(n2->location) << endl;
        }
        //else
            cout << p.manager.getString(n->nodes[i]->location) << endl;

    }


    ParsePrintVisitor printer{"/home/daniel/220.dot", p.manager};


    printer.printTree(*n);

}
//---------------------------------------------------------------------------
