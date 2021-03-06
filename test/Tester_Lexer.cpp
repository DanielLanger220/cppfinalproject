#include "gtest/gtest.h"

#include "../pljit/Lexer/Lexer.h"



using namespace std;
using namespace jit;

namespace jit::Tester_Lexer {

string code1 = "220:=abc+13   ;\nBEGIN\n\n:==:=PARAM/";
string code2 = "456?123\n";
string codeIncomplete1 = "123:abc\n";
string codeIncomplete2 = "123:";
string codeKeyword = "PARAM   BEGIN    VAR CONST RETURN END\n";
string codeSeparator = ".,;()\n";
string codeArithmetic = "+/:==-*\n";
string codeLiteral = "220 00284\n\n  \n\n\n 00000013\n";


TEST(Lexer, TestTokenType) {

    SourceCodeManager manager{code1};
    Lexer lex{code1, manager};

    auto tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Literal);
    EXPECT_EQ(manager.getString(tk->location), "220");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), ":=");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Identifier);
    EXPECT_EQ(manager.getString(tk->location), "abc");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), "+");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Literal);
    EXPECT_EQ(manager.getString(tk->location), "13");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(manager.getString(tk->location), ";");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(manager.getString(tk->location), "BEGIN");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), ":=");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), "=");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), ":=");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), false);
    EXPECT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(manager.getString(tk->location), "PARAM");

    tk = lex.nextToken();
    ASSERT_EQ(lex.checkForEndOfFile(), true);
    EXPECT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(manager.getString(tk->location), "/");
}

TEST(Lexer, TestIncompleteToken) {

    SourceCodeManager manager{codeIncomplete1};
    Lexer lex{codeIncomplete1, manager};

    auto tk = lex.nextToken();
    EXPECT_EQ(tk->tokentype, Token::TokenType::Literal);

    tk = lex.nextToken();
    EXPECT_EQ(tk, nullptr);


    SourceCodeManager manager2{codeIncomplete2};
    Lexer lex2{codeIncomplete2, manager2};

    tk = lex2.nextToken();
    EXPECT_EQ(tk->tokentype, Token::TokenType::Literal);

    tk = lex2.nextToken();
    EXPECT_EQ(tk, nullptr);
}

TEST(Lexer, TestErrorneousCharacter) {

    SourceCodeManager manager{code2};
    Lexer lex{code2, manager};

    auto tk = lex.nextToken();
    EXPECT_EQ(tk->tokentype, Token::TokenType::Literal);

    tk = lex.nextToken();
    EXPECT_EQ(tk, nullptr);
}

TEST(Lexer, TestLiteral) {

    SourceCodeManager manager{codeLiteral};
    Lexer lex{codeLiteral, manager};

    auto tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Literal);
    EXPECT_EQ(static_cast<Literal&>(*tk).value, 220);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Literal);
    EXPECT_EQ(static_cast<Literal&>(*tk).value, 284);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Literal);
    EXPECT_EQ(static_cast<Literal&>(*tk).value, 13);

}


TEST(Lexer, TestKeyword) {

    SourceCodeManager manager{codeKeyword};
    Lexer lex{codeKeyword, manager};

    auto tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::Parameter);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::Begin);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::Var);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::Constant);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::Ret);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Keyword);
    EXPECT_EQ(static_cast<Keyword&>(*tk).keywordtype, Keyword::KeywordType::End);


}


TEST(Lexer, TestSeparator) {

    SourceCodeManager manager{codeSeparator};
    Lexer lex{codeSeparator, manager};

    auto tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(static_cast<Separator&>(*tk).separatortype, Separator::SeparatorType::Dot);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(static_cast<Separator&>(*tk).separatortype, Separator::SeparatorType::Comma);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(static_cast<Separator&>(*tk).separatortype, Separator::SeparatorType::SemiColon);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(static_cast<Separator&>(*tk).separatortype, Separator::SeparatorType::OpenPar);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::Separator);
    EXPECT_EQ(static_cast<Separator&>(*tk).separatortype, Separator::SeparatorType::ClosePar);

}


TEST(Lexer, TestArithmeticOperator) {

    SourceCodeManager manager{codeArithmetic};
    Lexer lex{codeArithmetic, manager};

    auto tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::Plus);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::Div);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::VarAssign);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::Assign);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::Minus);

    tk = lex.nextToken();
    ASSERT_EQ(tk->tokentype, Token::TokenType::ArithmeticOperator);
    EXPECT_EQ(static_cast<ArithmeticOperator&>(*tk).arithmetictype, ArithmeticOperator::ArithmeticType::Mul);

}


} // namespace jit::Tester