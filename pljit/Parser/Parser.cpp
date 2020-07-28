#include "pljit/Parser/Parser.h"

namespace jit {

using namespace std;

using TokenType = Token::TokenType;
using SeparatorType = Separator::SeparatorType;
using ArithmeticType = ArithmeticOperator::ArithmeticType;
using KeywordType = Keyword::KeywordType;


std::unique_ptr<IdentifierNode> Parser::parseIdentifier() {

    auto currToken = nextToken();

    if (currToken->tokentype == TokenType::Identifier)
        return make_unique<IdentifierNode>(currToken->location);
    else {
        lookaheadToken = move(currToken);
        return nullptr;
    }
}

std::unique_ptr<LiteralNode> Parser::parseLiteral() {

    auto currToken = nextToken();

    if (currToken->tokentype == TokenType::Literal)
        return make_unique<LiteralNode>(currToken->location, static_cast<Literal*>(currToken.get())->value);
    else {
        lookaheadToken = move(currToken);
        return nullptr;
    }

}

std::unique_ptr<PrimaryExprNode> Parser::parsePrimaryExpr() {


    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};


    // Check for -> Identifier
    unique_ptr<ParseTreeNode> n = parseIdentifier();

    if (n) {

        nodes.push_back(move(n));
        return make_unique<PrimaryExprNode>(nodes[0]->location, move(nodes), PrimaryExprNode::SubType::Identifier);
    }

    // Check for -> Literal
    n = parseLiteral();
    if (n) {

        nodes.push_back(move(n));
        return make_unique<PrimaryExprNode>(nodes[0]->location, move(nodes), PrimaryExprNode::SubType::Literal);
    }


    // Check for -> "("  additive-expr  ")"

    // get the current token, either from lookahead or from lexer
    auto currToken = nextToken();

    if(checkForSeparatorToken(currToken.get()) == SeparatorType::OpenPar) {

         // Push "(" to the child nodes
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));

        // Parse the additive expression
        auto addexpr = parseAdditiveExpr();

        if (addexpr)
            nodes.push_back(move(addexpr));
        else
            return nullptr;

        // Check for ")"
        auto tk = nextToken();

        if (tk && checkForSeparatorToken(tk.get()) == SeparatorType::ClosePar) {
            nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));

            size_t range = manager.getabsolutePosition(tk->location) + 1 - manager.getabsolutePosition(nodes.front()->location);
            SourceCodeReference ref{nodes.front()->location, range};
            return make_unique<PrimaryExprNode>(ref, move(nodes), PrimaryExprNode::SubType::AdditiveExpr);

        }
        else {
            manager.printErrorMessage("error: ')' expected ...", tk->location);
            manager.printErrorMessage("... to match this '('", nodes[0]->location);
            return nullptr;
        }

    }
    else {

        manager.printErrorMessage("Error: Unexpected Token", currToken->location);
        return nullptr;
    }
}


std::unique_ptr<AdditiveExprNode> Parser::parseAdditiveExpr() {

    // Parse multiplicative expression
    auto multexpr = parseMultExpr();

    AdditiveExprNode::SubType subtype {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    if (!multexpr)
        return nullptr;
    else
        nodes.push_back(move(multexpr));


    // Check for optional ("+"|"-")  additive-expression
    auto tk = nextToken();
    auto t= checkForArithmeticToken(tk.get());

    if (t == ArithmeticType::Plus || t == ArithmeticType::Minus) {

        if (t == ArithmeticType::Plus)
            nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Plus));
        else
            nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Minus));

        auto addexpr = parseAdditiveExpr();

        if (!addexpr)
            return nullptr;

        subtype = AdditiveExprNode::SubType::Binary;
        nodes.push_back(move(addexpr));

    }
    else
        lookaheadToken = move(tk);


    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AdditiveExprNode>(ref, move(nodes), subtype) ;

}


std::unique_ptr<MultExprNode> Parser::parseMultExpr() {

    // Parse unary expression
    auto unaryexpr = parseUnaryExpr();

    MultExprNode::SubType subtype {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    if (!unaryexpr)
        return nullptr;
    else
        nodes.push_back(move(unaryexpr));


    // Check for optional ("*"|"/")  multiplicative-expression
    auto tk = nextToken();
    auto t = checkForArithmeticToken(tk.get());

    if (t == ArithmeticType::Mul || t == ArithmeticType::Div) {

        if (t == ArithmeticType::Mul)
            nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Mul));
        else
            nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Div));

        auto multexpr = parseMultExpr();

        if (!multexpr)
            return nullptr;

        subtype = MultExprNode::SubType::Binary;
        nodes.push_back(move(multexpr));

    }
    else
        lookaheadToken = move(tk);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<MultExprNode>(ref, move(nodes), subtype);
}

std::unique_ptr<UnaryExprNode> Parser::parseUnaryExpr() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    vector<unique_ptr<ParseTreeNode>> nodes{};

    UnaryExprNode::SubType subtype{UnaryExprNode::SubType::NoSign};

    auto t = checkForArithmeticToken(currToken.get());

    if (t == ArithmeticType::Plus) {

        subtype = UnaryExprNode::SubType::Plus;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    }
    else if (t == ArithmeticType::Minus) {
        subtype = UnaryExprNode::SubType::Minus;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    }
    else
        lookaheadToken = move(currToken);

    auto primaryexpr = parsePrimaryExpr();

    if (!primaryexpr)
        return nullptr;

    nodes.push_back(move(primaryexpr));

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<UnaryExprNode>(ref, move(nodes), subtype);
}


std::unique_ptr<AssignExprNode> Parser::parseAssignExpr() {

    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto tk = nextToken();

    // Check for identifier
    if (tk->tokentype != TokenType::Identifier) {
        lookaheadToken = move(tk);
        return nullptr;
    }

    nodes.push_back(make_unique<IdentifierNode>(tk->location));


    // Check for :=
    tk = nextToken();

    if (!tk)
        return nullptr;


    auto t = checkForArithmeticToken(tk.get());
    if (t != ArithmeticType::VarAssign) {

        manager.printErrorMessage("error: ':=' expected", tk->location);
        return nullptr;
    }

    nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));

    // Check for additive-expression
    auto addexpr = parseAdditiveExpr();

    if (!addexpr)
        return nullptr;

    nodes.push_back(move(addexpr));

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AssignExprNode>(ref, move(nodes));

}


std::unique_ptr<Statement> Parser::parseStatement() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    Statement::SubType subtype{Statement::SubType::Assign};


    // get the current token, either from lookahead of parent method or from lexer
    auto currToken = nextToken();

    if (!currToken)
    {
        manager.printErrorMessage("error: identifier or 'RETURN' expected", currToken->location);
        return nullptr;
    }

    // Check for RETURN
    auto t = checkForKeywordToken(currToken.get());
    if(t == KeywordType::Ret) {

        subtype = Statement::SubType::Return;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));

        // Check for additive-expression
        auto addexpr = parseAdditiveExpr();

        if (!addexpr)
            return nullptr;

        nodes.push_back(move(addexpr));

    }
    else {

        lookaheadToken = move(currToken);
        auto assignexpr = parseAssignExpr();

        if (!assignexpr)
            return nullptr;

        nodes.push_back(move(assignexpr));

    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<Statement>(ref, move(nodes), subtype);
}

std::unique_ptr<StatementList> Parser::parseStatementList() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto statement = parseStatement();

    if (!statement)
        return nullptr;

    nodes.push_back(move(statement));

    auto tk = nextToken();
    auto t = checkForSeparatorToken(tk.get());

    while(t == SeparatorType::SemiColon) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));

        statement = parseStatement();

        if(statement)
            nodes.push_back(move(statement));
        else
            return nullptr;

        tk = nextToken();
        t = checkForSeparatorToken(tk.get());
    }

    // move the falsely read token that was no ';' back to the lookahead
    lookaheadToken = move(tk);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<StatementList>(ref, move(nodes));
}


std::unique_ptr<CompoundStatement> Parser::parseCompoundStatement() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto tk = nextToken();


    // Check for 'BEGIN'
    if (!tk)
        return nullptr;

    auto t = checkForKeywordToken(tk.get());

    if (t == KeywordType::Begin) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));
    }
    else {
        manager.printErrorMessage("Error: 'BEGIN' expected", tk->location);
        return nullptr;
    }

    // Parse the compund statement
    auto statementlist = parseStatementList();

    if (!statementlist)
        return nullptr;


    nodes.push_back(move(statementlist));

    // Check for 'END'
    tk = nextToken();

    if (!tk)
        return nullptr;

    t = checkForKeywordToken(tk.get());

    if (t == KeywordType::End) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));
    }
    else {
        manager.printErrorMessage("Error: 'END' or ';' expected", tk->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<CompoundStatement>(ref, move(nodes));
}


std::unique_ptr<InitDeclNode> Parser::parseInitDecl() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    if (!currToken)
        return nullptr;

    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    // Check for -> Identifier
    if (currToken->tokentype == TokenType::Identifier) {
        nodes.push_back(make_unique<IdentifierNode>(currToken->location));
    }
    else {
        manager.printErrorMessage("error: Identifier expected", currToken->location);
        return nullptr;
    }

    // Check for -> =
    currToken = nextToken();

    if (!currToken)
        return nullptr;

    auto t = checkForArithmeticToken(currToken.get());
    if (t == ArithmeticType::Assign)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    else {
        manager.printErrorMessage("error: '=' expected", currToken->location);
        return nullptr;
    }

    // Check for -> Literal
    currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype == TokenType::Literal)
        nodes.push_back(make_unique<LiteralNode>(currToken->location, static_cast<Literal*>(currToken.get())->value));
    else {
        manager.printErrorMessage("error: literal expected", currToken->location);
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<InitDeclNode>(ref, std::move(nodes));
}


std::unique_ptr<InitDeclListNode> Parser::parseInitDeclList() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto initdecl = parseInitDecl();

    if (!initdecl)
        return nullptr;

    nodes.push_back(move(initdecl));

    auto tk = nextToken();
    auto t = checkForSeparatorToken(tk.get());

    while(t == SeparatorType::Comma) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location, GenericTerminalNode::SubType::Other));

        initdecl = parseInitDecl();

        if(initdecl)
            nodes.push_back(move(initdecl));
        else
            return nullptr;

        tk = nextToken();
        t = checkForSeparatorToken(tk.get());
    }

    // move the falsely read token that was no ';' back to the lookahead
    lookaheadToken = move(tk);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<InitDeclListNode>(ref, move(nodes));

}


std::unique_ptr<DeclListNode> Parser::parseDeclList() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    if (!currToken)
        return nullptr;

    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    // Check for -> Identifier
    if (currToken->tokentype == TokenType::Identifier) {
        nodes.push_back(make_unique<IdentifierNode>(currToken->location));
    }
    else {
        manager.printErrorMessage("error: Identifier expected", currToken->location);
        return nullptr;
    }



    currToken = nextToken();
    auto t = checkForSeparatorToken(currToken.get());

    while(t == SeparatorType::Comma) {

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));

        currToken = nextToken();

        if (currToken->tokentype == TokenType::Identifier) {
            nodes.push_back(make_unique<IdentifierNode>(currToken->location));
        }
        else {
            manager.printErrorMessage("error: Identifier expected", currToken->location);
            return nullptr;
        }

        currToken = nextToken();
        t = checkForSeparatorToken(currToken.get());
    }

    // move the falsely read token that was no ';' back to the lookahead
    lookaheadToken = move(currToken);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<DeclListNode>(ref, move(nodes));

}


optional<std::unique_ptr<ParamDeclNode>> Parser::parseParamDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'PARAM'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Parameter)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    else {

        lookaheadToken = move(currToken);
        return nullopt;
    }

    // Parse the decl-list
    auto decllist = parseDeclList();

    if (!decllist)
        return nullptr;

    nodes.push_back(move(decllist));

    // Check for ';'
    currToken = nextToken();

    if (!currToken)
        return nullptr;

    auto t2 = checkForSeparatorToken(currToken.get());

    if (t2 == SeparatorType::SemiColon) {

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<ParamDeclNode>(ref, move(nodes));


}

optional<std::unique_ptr<VarDeclNode>> Parser::parseVarDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'VAR'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Var)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    else {
        lookaheadToken = move(currToken);
        return nullopt;
    }

    // Parse the decl-list
    auto decllist = parseDeclList();

    if (!decllist)
        return nullptr;

    nodes.push_back(move(decllist));

    // Check for ';'
    currToken = nextToken();

    if (!currToken)
        return nullptr;

    auto t2 = checkForSeparatorToken(currToken.get());

    if (t2 == SeparatorType::SemiColon) {

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<VarDeclNode>(ref, move(nodes));


}
optional<std::unique_ptr<ConstDeclNode>> Parser::parseConstDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'CONST'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Constant)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    else {
        lookaheadToken = move(currToken);
        return nullopt;
    }

    // Parse the init-decl-list
    auto initdecllist = parseInitDeclList();

    if (!initdecllist)
        return nullptr;

    nodes.push_back(move(initdecllist));

    // Check for ';'
    currToken = nextToken();

    if (!currToken)
        return nullptr;

    auto t2 = checkForSeparatorToken(currToken.get());

    if (t2 == SeparatorType::SemiColon) {

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<ConstDeclNode>(ref, move(nodes));


}


std::unique_ptr<FuncDeclNode> Parser::parseFunction() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    bool hasParam{false};
    bool hasVar{false};
    bool hasConst{false};

    auto paramdecl = parseParamDecl();
    if (paramdecl) {
        if (!paramdecl.value())
            return nullptr;

        cout << "Param declaration found\n";

        hasParam = true;
        nodes.push_back(move(paramdecl.value()));
    }

    auto vardecl = parseVarDecl();
    if (vardecl) {
        if(!vardecl.value())
            return nullptr;

        cout << "Var declaration found\n";


        hasVar = true;
        nodes.push_back(move(vardecl.value()));
    }

    auto constdecl = parseConstDecl();
    if (constdecl) {
        if(!constdecl.value())
            return nullptr;

        cout << "Constant declaration found\n";

        hasConst = true;
        nodes.push_back(move(constdecl.value()));
    }

    auto compstatement = parseCompoundStatement();
    if (!compstatement)
        return nullptr;

    nodes.push_back(move(compstatement));


    // Check for final .
    auto currToken = nextToken();
    auto t = checkForSeparatorToken(currToken.get());
    if (t == SeparatorType::Dot)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other));
    else {
        manager.printErrorMessage("error: '.' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};


    return make_unique<FuncDeclNode>(ref, move(nodes), hasParam, hasVar, hasConst);

}


// **********************************************
// Helper methods
// **********************************************

std::optional<KeywordType> Parser::checkForKeywordToken(const Token* tk)
{
    if (!tk || tk->tokentype != TokenType::Keyword)
        return nullopt;

    return (make_optional(static_cast<const Keyword*>(tk)->keywordtype));

}

std::optional<ArithmeticType> Parser::checkForArithmeticToken(const Token* tk)
{
    if (!tk || tk->tokentype != TokenType::ArithmeticOperator)
        return nullopt;

    return (make_optional(static_cast<const ArithmeticOperator*>(tk)->arithmetictype));

}

std::optional<SeparatorType> Parser::checkForSeparatorToken(const Token* tk)
{

    if (!tk || tk->tokentype != TokenType::Separator)
        return nullopt;

    return (make_optional(static_cast<const Separator*>(tk)->separatortype));

}

} // namespace jit
