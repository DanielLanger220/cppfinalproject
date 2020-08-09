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

    ASSERT_EQ(f->nodes[1]->nodetype, ParseTreeNode::Type::CompundStatement);
    CompoundStatement& comp = static_cast<CompoundStatement&>(*f->nodes[1]);

    EXPECT_EQ(comp.nodes.size(), 3);

    ASSERT_EQ(comp.nodes[1]->nodetype, ParseTreeNode::Type::StatementList);
    StatementList& sl = static_cast<StatementList&>(*comp.nodes[1]);

    ASSERT_EQ(sl.nodes.size(), 7);

    for(int i = 0; i < 6; i += 2)
    {
        ASSERT_EQ(sl.nodes[i]->nodetype, ParseTreeNode::Type::Statement);

        auto* st = static_cast<Statement*>(sl.nodes[i].get());
        EXPECT_EQ(st->subtype, Statement::SubType::Assign);

        EXPECT_EQ(sl.nodes[i+1]->nodetype, ParseTreeNode::Type::GenericTerminal);
    }

    Statement& st = static_cast<Statement&>(*sl.nodes[6]);

    ASSERT_EQ(st.nodes.size(), 2);
    EXPECT_EQ( st.subtype, Statement::SubType::Return);

    EXPECT_EQ(st.nodes[0]->nodetype, ParseTreeNode::Type::GenericTerminal);
    EXPECT_EQ(st.nodes[1]->nodetype, ParseTreeNode::Type::AdditiveExpr);

}


} // namespace jit::Tester_Parser