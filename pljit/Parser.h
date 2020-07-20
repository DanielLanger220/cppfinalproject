#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H

#include "Lexer.h"
#include "TreeNode.h"

namespace jit {

class Parser {

    public:

    explicit Parser(std::string& sourcecode) : manager{sourcecode}, lex{sourcecode, manager} {}

    std::unique_ptr<FuncDeclNode> parseFunction();




    //private:
    SourceCodeManager manager;
    Lexer lex;


    std::unique_ptr<DeclListNode> parseDeclList();

    std::unique_ptr<ConstDeclNode> parseConstDecl();
    std::unique_ptr<VarDeclNode> parseVarDecl();
    std::unique_ptr<ParamDeclNode> parseParamDecl();

    std::unique_ptr<CompoundStatement> parseCompundStatement();
    std::unique_ptr<StatementList> parseStatementList();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<AssignExprNode> parseAssignExpr();

    std::unique_ptr<AdditiveExprNode> parseAdditiveExpr();
    std::unique_ptr<MultExprNode> parseMultExpr();
    std::unique_ptr<UnaryExprNode> parseUnaryExpr();
    std::unique_ptr<PrimaryExprNode> parsePrimaryExpr();


    private:
    std::unique_ptr<Token> lookaheadToken{ nullptr};
    std::unique_ptr<Token> nextToken() { return lookaheadToken ? std::move(lookaheadToken) : lex.nextToken();}

    // Helper methods
    static bool checkForAddSub(const Token* tk);
    static bool checkForMulDiv(const Token* tk);
    static bool checkForOpenPar(const Token* tk);
    static bool checkForClosePar(const Token* tk);
    static bool checkForVarAssign(const Token* tk);
    static bool checkForReturn(const Token* tk);
    static bool checkForSemiColon(const Token* tk);
    static bool checkForBegin(const Token* tk);
    static bool checkForEnd(const Token* tk);


};



} // namespace jit
#endif //PLJIT_PARSER_H
