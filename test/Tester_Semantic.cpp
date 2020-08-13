#include "gtest/gtest.h"

#include "../pljit/SemanticAnalysis/SemanticAnalyser.h"
#include "../pljit/Parser/Parser.h"

#include <memory>

using namespace std;
using namespace jit;


namespace jit::Tester_Semantic {


string code1 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, e = 2;\n"
               "BEGIN\n"
               "b := a + u;\n"      // u ==> undedlared identifier
               "c := 13;\n"
               "RETURN b\n"
               "END.\n";

TEST(SemanticAnalysis, UndeclaredIdentifier) {

    // Parse the sourcecode
    SourceCodeManager manager{code1};
    Parser parser{code1, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_EQ(function, nullptr);
}


string code2 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, e = 2;\n"
               "BEGIN\n"
               "d := a;\n"      // Assign to constant d
               "c := 13;\n"
               "RETURN b\n"
               "END.\n";

TEST(SemanticAnalysis, AssignToConst) {

    // Parse the sourcecode
    SourceCodeManager manager{code2};
    Parser parser{code2, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_EQ(function, nullptr);
}


string code3 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, e = 2;\n"
               "BEGIN\n"
               "b := a + b;\n"      // b ==> uninitialised variable on the right hand side
               "c := 13;\n"
               "RETURN b\n"
               "END.\n";

TEST(SemanticAnalysis, UseUninitialisedVariable) {

    // Parse the sourcecode
    SourceCodeManager manager{code3};
    Parser parser{code3, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_EQ(function, nullptr);
}


string code4 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, e = 2;\n"
               "BEGIN\n"
               "b := a + d;\n"
               "c := 13\n"
               "END.\n";

TEST(SemanticAnalysis, MissingReturn) {

    // Parse the sourcecode
    SourceCodeManager manager{code4};
    Parser parser{code4, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_EQ(function, nullptr);
}


string code5 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, b = 42, e = 2;\n"  // b ==> declared twice
               "BEGIN\n"
               "b := a + d;\n"
               "c := 13;\n"
               "RETURN b\n"
               "END.\n";

TEST(SemanticAnalysis, IdentifierDeclaredTwice) {

    // Parse the sourcecode
    SourceCodeManager manager{code5};
    Parser parser{code5, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_EQ(function, nullptr);
}

string code6 = "PARAM a;\n"
               "VAR b, c;\n"
               "CONST d = 1, e = 2;\n"
               "BEGIN\n"
               "b := a + (-d);\n"
               "c := e * a;\n"
               "RETURN b * c\n"
               "END.\n";

TEST(SemanticAnalysis, TestAst) {

    // Parse the sourcecode
    SourceCodeManager manager{code6};
    Parser parser{code6, manager};

    auto parsetree = parser.parseFunction();

    ASSERT_NE(parsetree, nullptr);

    // Do the semantical analysis
    SemanticAnalyser seman{manager, *parsetree};

    auto function = seman.analyseFunction();
    ASSERT_NE(function, nullptr);


    EXPECT_EQ(function->nofidentifiers, 3);
    EXPECT_EQ(function->nofparameters, 1);
    EXPECT_EQ(function->nofvariables, 2);

    auto& st = *function->statementlist;
    EXPECT_EQ(st.statements.size(), 3);

    // b := a + (-d);
    auto* s = st.statements[0].get();
    EXPECT_EQ(s->subtype, AstStatement::SubType::AstAssignment);
    AstArithmeticExpression* ae = static_cast<AstAssignment*>(s)->rhs.get();
    ASSERT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Binary);
    EXPECT_EQ(static_cast<AstBinaryArithmeticExpression*>(ae)->op, AstBinaryArithmeticExpression::ArithmeticOperation::Plus);

        // a
    AstArithmeticExpression* ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->lhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Identifier);

        // (-d)
    ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->rhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Unary);
    ae2 = static_cast<AstUnaryArithmeticExpression*>(ae2)->subexpr.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Literal); // constant d should have been made a literal


    // c := e * a;
    s = st.statements[1].get();
    EXPECT_EQ(s->subtype, AstStatement::SubType::AstAssignment);
    ae = static_cast<AstAssignment*>(s)->rhs.get();
    ASSERT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Binary);
    EXPECT_EQ(static_cast<AstBinaryArithmeticExpression*>(ae)->op, AstBinaryArithmeticExpression::ArithmeticOperation::Mul);

        // e
    ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->lhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Literal); // constant e should have been made a literal

        // a
    ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->rhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Identifier);


    // RETURN b * c
    s = st.statements[2].get();
    EXPECT_EQ(s->subtype, AstStatement::SubType::AstReturn);
    ae = static_cast<AstReturn*>(s)->returnvalue.get();
    ASSERT_EQ(ae->subtype, AstArithmeticExpression::Subtype::Binary);
    EXPECT_EQ(static_cast<AstBinaryArithmeticExpression*>(ae)->op, AstBinaryArithmeticExpression::ArithmeticOperation::Mul);

    // b
    ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->lhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Identifier);

    // c
    ae2 = static_cast<AstBinaryArithmeticExpression*>(ae)->rhs.get();
    EXPECT_EQ(ae2->subtype, AstArithmeticExpression::Subtype::Identifier);


}


} // namespace jit::Tester_Semantic