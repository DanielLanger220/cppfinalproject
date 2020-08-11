#include "gtest/gtest.h"

#include "../pljit/Pljit.h"
#include "../pljit/Parser/Parser.h"
#include "../pljit/SemanticAnalysis/SemanticAnalyser.h"
#include "../pljit/SemanticAnalysis/DeadCodeOpt.h"
#include "../pljit/SemanticAnalysis/ConstantPropOpt.h"

using namespace std;
using namespace jit;


namespace jit::Tester_Optimisation {


string code1 = "PARAM a, b;\n"
               "VAR c;\n"
               "CONST d = 220;\n"
               "BEGIN\n"
               "c := (a + b) * d;\n"
               "RETURN (a - 2 * b) + 3 * c\n"
               "END.\n";

string code2 = "PARAM a, b;\n"
               "VAR c, d;\n"
               "BEGIN\n"
               "c := a * a;\n"
               "d := b * (-b)\n;"
               "RETURN c + d;\n"
               "RETURN a;\n"
               "a := 1 + 2;\n"
               "RETURN b;\n"
               "RETURN c\n"
               "END.";

string code3 = "PARAM a, b;\n"                      // 1
               "VAR c, d;\n"                        // 2
               "CONST e = 220;\n"                   // 3
               "BEGIN\n"                            // 4
               "c := 13;\n"                         // 5
               "d := c * e + 4;"                    // 6
               "RETURN (a - 2 * b) + 3 * c + d\n"   // 7
               "END.\n";

TEST(ConstantPropagationOptimisation, Test1) {

    // Parse the sourcecode
    SourceCodeManager manager{code3};
    Parser parser{code3, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);


    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_NE(function, nullptr);

    // Optimise the function object by constant propagation

    ConstantPropOpt constopt{};
    function->optimise(constopt);

    AstStatementList& statements = static_cast<AstStatementList&>(*function->statementlist);

    // d := c * e + 4 ==> Check if right hand side has been optimised into a single literal node
    AstStatement* st = statements.statements[1].get();
    EXPECT_EQ(st->subtype, AstStatement::SubType::AstAssignment);
    AstArithmeticExpression* ae = static_cast<AstAssignment*>(st)->rhs.get();
    cout << manager.getString(ae->location) << endl;
    EXPECT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Literal);

    // RETURN (a - 2 * b) + 3 * c + d ==> Check, if '3 * c + d' has been merged into a single literal node
    st = statements.statements[2].get();
    EXPECT_EQ(st->subtype, AstStatement::SubType::AstReturn);
    ae = static_cast<AstReturn*>(st)->returnvalue.get();    // ae == '(a - 2 * b) + 3 * c + d' ==> Binary arithmetic expression
    ASSERT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Binary);
    ae = static_cast<AstBinaryArithmeticExpression*>(ae)->rhs.get(); // ae == '3 * c + d' ==> Check, if this is a literal
    cout << manager.getString(ae->location) << endl;
    EXPECT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Literal);

}

/*TEST(ConstantPropagationOptimisation, Test2) {

    // Parse the sourcecode
    SourceCodeManager manager{code2};
    Parser parser{code2, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);


    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    bool b = seman.createTable();
    ASSERT_EQ(b, true);

    auto function = seman.analyseFunction();
    ASSERT_NE(function, nullptr);

    AstStatementList& statements = static_cast<AstStatementList&>(*function->statementlist);

    EXPECT_EQ(statements.statements.size(), 7);

    // Dead-Code optimise the function object

    DeadCodeOpt deadcodeopt{};
    function->optimise(deadcodeopt);

    EXPECT_EQ(statements.statements.size(), 3);

}*/


TEST(DeadCodeOptimisation, NoDeadCodeTest) {

    // Parse the sourcecode
    SourceCodeManager manager{code1};
    Parser parser{code1, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);


    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_NE(function, nullptr);


    // Dead-Code optimise the function object

    DeadCodeOpt deadcodeopt{};
    function->optimise(deadcodeopt);

    AstStatementList& statements = static_cast<AstStatementList&>(*function->statementlist);

    EXPECT_EQ(statements.statements.size(), 2);

}

TEST(DeadCodeOptimisation, DeadCodeTest) {

    // Parse the sourcecode
    SourceCodeManager manager{code2};
    Parser parser{code2, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);


    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_NE(function, nullptr);

    AstStatementList& statements = static_cast<AstStatementList&>(*function->statementlist);

    EXPECT_EQ(statements.statements.size(), 7);

    // Dead-Code optimise the function object

    DeadCodeOpt deadcodeopt{};
    function->optimise(deadcodeopt);

    EXPECT_EQ(statements.statements.size(), 3);

}



} // namespace jit::Tester_Optimisation