#include "gtest/gtest.h"

#include "../pljit/Evaluation/EvalInstance.h"
#include "../pljit/Parser/Parser.h"
#include "../pljit/SemanticAnalysis/SemanticAnalyser.h"


using namespace std;
using namespace jit;


namespace jit::Tester_Evaluation {

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
               "RETURN b;\n"
               "RETURN c\n"
               "END.";


TEST(Evaluation, code1) {

    SourceCodeManager manager{code1};
    Parser p{code1, manager};


    auto f = p.parseFunction();
    ASSERT_NE(f, nullptr);

    SemanticAnalyser sa{manager, *f};

    auto ast = sa.analyseFunction();
    ASSERT_NE(ast, nullptr);

    EvalInstance ev{*ast, manager};

    optional<int64_t> result = ev.evaluate({42, 17});
    EXPECT_EQ(result.value(), 38948);

    result = ev.evaluate({3,-4});
    EXPECT_EQ(result.value(), -649);

    result = ev.evaluate({1});
    EXPECT_EQ(result, nullopt);

    result = ev.evaluate({1,2,3});
    EXPECT_EQ(result, nullopt);

}

TEST(Evaluation, code2) {

    SourceCodeManager manager{code2};
    Parser p{code2, manager};

    auto f = p.parseFunction();
    ASSERT_NE(f, nullptr);

    SemanticAnalyser sa{manager, *f};

    auto ast = sa.analyseFunction();
    ASSERT_NE(ast, nullptr);

    EvalInstance ev{*ast, manager};

    optional<int64_t> result = ev.evaluate({3, -4});
    EXPECT_EQ(result.value(), -7);

    result = ev.evaluate({5, 10});
    EXPECT_EQ(result.value(), -75);

    result = ev.evaluate({-9, -7});
    EXPECT_EQ(result.value(), 32);

}




} // namespace jit::Tester_Evaluation