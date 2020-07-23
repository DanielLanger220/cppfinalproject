#include <iostream>

#include "SourceCodeManager.h"
#include "pljit/Lexer/Lexer.h"
#include "pljit/Parser/ParsePrintVisitor.h"
#include "pljit/Parser/Parser.h"
#include "SemanticAnalysis/SemanticAnalyzer.h"
#include "SemanticAnalysis/AstPrintVisitor.h"
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

  /*  std::string code3 = "PARAM Kata, Jynx, Sandra;\n"
                        "VAR Steffi;\n"
                        "CONST Sophie = 284, Daniel = 220, Jill = 99;\n"
                        "BEGIN \n"
                        "Sophie := 220;\n"
                        "Sophie := 220 + 284;\n"
                        "RETURN 284\n"
                        "END.\n"
                        "\n";

*/
    std::string code3 = "PARAM Kata, Jynx, Sandra;\n"
                        "VAR Steffi;\n"
                        "CONST Sophie = 284, Daniel = 220, Jill = 99;\n"
                        "BEGIN\n"
                        "RETURN 220;"
                        "Jynx := (220 + 13)  *((-300));\n"
                        "Steffi:= 13 - 27 + 220\n"
                        "END.\n"
                        "\n";



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




    CompoundStatement& comp = static_cast<CompoundStatement&>(*n->nodes[3]);

    StatementList& sl = static_cast<StatementList&>(*comp.nodes[1]);


    Statement& s = static_cast<Statement&>(*sl.nodes[0]);

    //AssignExprNode& ae = static_cast<AssignExprNode&>(*s.nodes[0]);

    //AdditiveExprNode& addexpr = static_cast<AdditiveExprNode&>(*ae.nodes[2]);
/*
    if (addexpr.nodetype == ParseTreeNode::Type::AdditiveExpr)
        cout << "Juhu Sophie" << endl;

    cout << p.manager.getString(addexpr.location) << endl;
*/

    SemanticAnalyzer seman{p.manager, *n};

    seman.createTables();

    //auto t = seman.createSymbolTable();


    //const auto ast = seman.analyzeExpression(addexpr);


    //if (ast == nullptr)
    //    cout << "GRRRRRRRRR\n";

    //AstBinaryArithmeticExpression& lhs1 = static_cast<AstBinaryArithmeticExpression&>(*static_cast<AstBinaryArithmeticExpression&>(*ast).lhs);


    //AstBinaryArithmeticExpression& rhs1 = static_cast<AstBinaryArithmeticExpression&>(*lhs1.rhs);

    //cout << (int)rhs1.subtype << endl;



    //cout << p.manager.getString(rhs1.location) << endl;

/*


    auto aststatement = seman.analyzeStatement(s);

    astprinter.printStatement(*aststatement);
*/

    std::unique_ptr<AstFunction> func = seman.analyseFunction(*n);

    AstPrintVisitor astprinter{"/home/daniel/284.dot", p.manager};

    astprinter.printFunction(*func);

}
//---------------------------------------------------------------------------
