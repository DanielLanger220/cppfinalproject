#ifndef PLJIT_PARSER_H
#define PLJIT_PARSER_H

#include <optional>

#include "ParseTreeNode.h"
#include "pljit/Lexer/Lexer.h"

namespace jit {

class Parser {

    public:

    // Constructor
    Parser(const std::string& sourcecode, const SourceCodeManager& manager) : manager{manager}, lex{sourcecode, manager} {}

    // parseFunction                Parses the source code and, if successfull, returns a pointer to the root node of the created parse tree
    std::unique_ptr<FuncDeclNode> parseFunction();

    private:

    const SourceCodeManager& manager;       // A reference to the source code manager
    Lexer lex;                              // The lexer that is used within the parser


    // Parser methods to parse Separator-, Keyword- and ArithemticOperator token. The methods check if the next token matches the token given as parameter.
    // The mandatory-flag indicates whether the token is mandatory or optional at that positon
    std::unique_ptr<GenericTerminalNode> parseSeparator(Separator::SeparatorType t, bool mandatory = false);
    std::unique_ptr<GenericTerminalNode> parseKeyword(Keyword::KeywordType t, bool mandatory = false);
    std::unique_ptr<GenericTerminalNode> parseArithmeticOperator(ArithmeticOperator::ArithmeticType t, bool mandatory = false);

    // Parser methods to parse an identifier and a literal with a flag indicating whether the token is mandatory or optional
    std::unique_ptr<IdentifierNode> parseIdentifier(bool mandatory = false);
    std::unique_ptr<LiteralNode> parseLiteral(bool mandatory = false);

    // Parser methods for the arithmetic expressions with a flag indicating whether the expression is mandatory or optional
    std::unique_ptr<PrimaryExprNode> parsePrimaryExpr(bool mandatory = false);
    std::unique_ptr<UnaryExprNode> parseUnaryExpr(bool mandatory = false);
    std::unique_ptr<MultExprNode> parseMultExpr(bool mandatory = false);
    std::unique_ptr<AdditiveExprNode> parseAdditiveExpr(bool mandatory = false);

    // parseAssignExpr          Parses an assignment expression with flag indicating whether the expression is mandatory or optional
    std::unique_ptr<AssignExprNode> parseAssignExpr(bool mandatory = false);

    // Parser methods to parse the statements and the compound statement
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<StatementList> parseStatementList();
    std::unique_ptr<CompoundStatement> parseCompoundStatement();

    // Parser methods to parse the declaration- and initialisation lists for the declarations
    std::unique_ptr<InitDeclNode> parseInitDecl();
    std::unique_ptr<DeclListNode> parseDeclList();
    std::unique_ptr<InitDeclListNode> parseInitDeclList();

    // Parser methods to parse the declarations of the parameters, variables and constants
    std::optional<std::unique_ptr<ParamDeclNode>> parseParamDecl();
    std::optional<std::unique_ptr<VarDeclNode>> parseVarDecl();
    std::optional<std::unique_ptr<ConstDeclNode>> parseConstDecl();

    private:

    // Here, tokens are placed when they were falsely taken from the lexer to try an alternative
    // from a non-terminal symbol which yields in a false alternative
    std::unique_ptr<Token> lookaheadToken{ nullptr};

    // nextToken                If the lookahead token is filled with a token, returns this token, otherwise takes the next token from the lexer
    std::unique_ptr<Token> nextToken() { return lookaheadToken ? std::move(lookaheadToken) : lex.nextToken();}

};



} // namespace jit
#endif //PLJIT_PARSER_H
