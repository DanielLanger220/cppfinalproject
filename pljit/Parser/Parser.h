#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H

#include <optional>

#include "ParseTreeNode.h"
#include "pljit/Lexer/Lexer.h"

namespace jit {

class Parser {

    public:

    explicit Parser(std::string& sourcecode) : manager{sourcecode}, lex{sourcecode, manager} {}

    std::unique_ptr<FuncDeclNode> parseFunction();

    //private:
    SourceCodeManager manager;
    Lexer lex;


    std::optional<std::unique_ptr<ParamDeclNode>> parseParamDecl();
    std::optional<std::unique_ptr<VarDeclNode>> parseVarDecl();
    std::optional<std::unique_ptr<ConstDeclNode>> parseConstDecl();

    std::unique_ptr<DeclListNode> parseDeclList();
    std::unique_ptr<InitDeclNode> parseInitDecl();
    std::unique_ptr<InitDeclListNode> parseInitDeclList();

    std::unique_ptr<CompoundStatement> parseCompoundStatement();
    std::unique_ptr<StatementList> parseStatementList();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<AssignExprNode> parseAssignExpr();

    std::unique_ptr<AdditiveExprNode> parseAdditiveExpr();
    std::unique_ptr<MultExprNode> parseMultExpr();
    std::unique_ptr<UnaryExprNode> parseUnaryExpr();
    std::unique_ptr<PrimaryExprNode> parsePrimaryExpr();

    std::unique_ptr<IdentifierNode> parseIdentifier();
    std::unique_ptr<LiteralNode> parseLiteral();



    private:

    // Here, tokens are placed when they were falsely taken from the lexer to try an alternative
    // from a non-terminal symbol which yields in a false alternative
    std::unique_ptr<Token> lookaheadToken{ nullptr};


    // Helper methods

    // nextToken                If the lookahead token is filled with a token, returns this token, otherwise takes the next token from the lexer
    std::unique_ptr<Token> nextToken() { return lookaheadToken ? std::move(lookaheadToken) : lex.nextToken();}


    static std::optional<Keyword::KeywordType> checkForKeywordToken(const Token* tk);
    static std::optional<ArithmeticOperator::ArithmeticType> checkForArithmeticToken(const Token* tk);
    static std::optional<Separator::SeparatorType> checkForSeparatorToken(const Token* tk);

};



} // namespace jit
#endif //PLJIT_PARSER_H
