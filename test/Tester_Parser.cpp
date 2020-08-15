#include "gtest/gtest.h"

#include "../pljit/Parser/Parser.h"


using namespace std;
using namespace jit;

namespace jit::Tester_Parser {

string code1 = "VAR a, b, c;\n"
               "BEGIN\n"
               "a := 42;\n"
               "b := 57;\n"
               "c := a * b;\n"
               "RETURN c\n"
               "END.\n";

TEST(Parser, code1) {

    SourceCodeManager manager{code1};
    Parser parser{code1, manager};

    auto f = parser.parseFunction();
    ASSERT_NE(f, nullptr);


    EXPECT_EQ(f->nodes.size(), 3);

    // Check Compound-Statement
    ASSERT_EQ(f->nodes[1]->nodetype, ParseTreeNode::Type::CompundStatement);
    CompoundStatement& comp = static_cast<CompoundStatement&>(*f->nodes[1]);
    EXPECT_EQ(comp.nodes.size(), 3);

    // Check Statement-List
    ASSERT_EQ(comp.nodes[1]->nodetype, ParseTreeNode::Type::StatementList);
    StatementList& sl = static_cast<StatementList&>(*comp.nodes[1]);
    ASSERT_EQ(sl.nodes.size(), 7);

    // Check the assignments
    for(int i = 0; i < 6; i += 2)
    {
        ASSERT_EQ(sl.nodes[i]->nodetype, ParseTreeNode::Type::Statement);

        auto* st = static_cast<Statement*>(sl.nodes[i].get());
        EXPECT_EQ(st->subtype, Statement::SubType::Assign);

        EXPECT_EQ(sl.nodes[i+1]->nodetype, ParseTreeNode::Type::GenericTerminal);
    }

    // Check the return statement
    Statement& st = static_cast<Statement&>(*sl.nodes[6]);

    ASSERT_EQ(st.nodes.size(), 2);
    EXPECT_EQ( st.subtype, Statement::SubType::Return);

    EXPECT_EQ(st.nodes[0]->nodetype, ParseTreeNode::Type::GenericTerminal);
    EXPECT_EQ(st.nodes[1]->nodetype, ParseTreeNode::Type::AdditiveExpr);

}

string code2 = "VAR a, b c;\n"      // Error: missing ',' in optional variable declaration
               "BEGIN\n"
               "a := 42;\n"
               "b := 57;\n"
               "c := a * b;\n"
               "RETURN c\n"
               "END.\n";

TEST(Parser, code2) {

    SourceCodeManager manager{code2};
    Parser parser{code2, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}


string code3 = "VAR a, b, c;\n"
               "BEGIN\n"
               "a := 42;\n"
               "b := 57;\n"
               "c := a * (b + c * b;\n"     // Error: missing ')'
               "RETURN c\n"
               "END.\n";

TEST(Parser, code3) {

    SourceCodeManager manager{code3};
    Parser parser{code3, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}

string code4 = "VAR a, b, c;\n"
               "BEGIN\n"
               "a := 42;\n"
               "b = 57;\n"          // Error: '=' instead of ':='
               "c := a * b;\n"
               "RETURN c\n"
               "END.\n";

TEST(Parser, code4) {

    SourceCodeManager manager{code4};
    Parser parser{code4, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}

string code5 = "VAR a, b, c;\n"
               "BEGIN\n"
               "a := 42;\n"
               "b := 57;\n"
               "c := VAR;\n"        // Error: Keyword instead of additive-expression
               "RETURN c\n"
               "END.\n";

TEST(Parser, code5) {

    SourceCodeManager manager{code5};
    Parser parser{code5, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}

string code6 = "VAR a, b, c;\n"             // Error: Missing 'BEGIN'
               "a := 42;\n"
               "b := 57;\n"
               "c := a * b;\n"
               "RETURN c\n"
               "END.\n";

TEST(Parser, code6) {

    SourceCodeManager manager{code6};
    Parser parser{code6, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}

string code7 = "VAR a, b, c;\n"
               "BEGIN\n"
               "a := 42;\n"
               "b := 57\n"          // Error: Missing ';'
               "c := a * b;\n"
               "RETURN c\n"
               "END.\n";

TEST(Parser, code7) {

    SourceCodeManager manager{code7};
    Parser parser{code7, manager};

    auto f = parser.parseFunction();
    ASSERT_EQ(f, nullptr);

}




} // namespace jit::Tester_Parser