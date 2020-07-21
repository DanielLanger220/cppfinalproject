#include "Parser.h"

namespace jit {

using namespace std;



std::unique_ptr<IdentifierNode> Parser::parseIdentifier() {

    auto currToken = nextToken();

    if (currToken->tokentype == TokenType::Identifier)
        return make_unique<IdentifierNode>(currToken->location, static_cast<Identifier*>(currToken.get())->id);
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


    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    // vector of child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    // Check for -> Identifier
    if (currToken->tokentype == TokenType::Identifier) {

        nodes.push_back(make_unique<IdentifierNode>(currToken->location, static_cast<Identifier*>(currToken.get())->id));
        return make_unique<PrimaryExprNode>(PrimaryExprNode::SubType::Identifier, move(nodes), currToken->location);

    }
    // Check for -> Literal
    else if (currToken->tokentype == TokenType::Literal) {

        nodes.push_back(make_unique<LiteralNode>(currToken->location, static_cast<Literal*>(currToken.get())->value));
        return make_unique<PrimaryExprNode>(PrimaryExprNode::SubType::Literal, move(nodes), currToken->location);

    }
    // Check for -> "("  additive-expr  ")"
    //else if (checkForOpenPar(currToken.get())) {
    else if(checkForSeparatorToken(currToken.get()) == SeparatorType::OpenPar) {

         // Push "(" to the child nodes
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));

        // Parse the additive expression
        auto addexpr = parseAdditiveExpr();

        if (addexpr)
            nodes.push_back(move(addexpr));
        else
            return nullptr;

        // Check for ")"
        auto n = nextToken();

        if (n && checkForSeparatorToken(n.get()) == SeparatorType::ClosePar) {
            nodes.push_back(make_unique<GenericTerminalNode>(n->location));

            size_t range = manager.getabsolutePosition(n->location) + 1 - manager.getabsolutePosition(nodes.front()->location);
            SourceCodeReference ref{nodes.front()->location, range};
            return make_unique<PrimaryExprNode>(PrimaryExprNode::SubType::AdditiveExpr, move(nodes), ref);

        }
        else {
            manager.printErrorMessage("error: ')' expected ...", n->location);
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
    vector<unique_ptr<TreeNode>> nodes{};

    if (!multexpr)
        return nullptr;
    else
        nodes.push_back(move(multexpr));


    // Check for optional ("+"|"-")  additive-expression
    auto tk = nextToken();
    auto t= checkForArithmeticToken(tk.get());

    if (t == ArithmeticType::Plus || t == ArithmeticType::Minus) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

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

    return make_unique<AdditiveExprNode>(subtype, move(nodes), ref);

}


std::unique_ptr<MultExprNode> Parser::parseMultExpr() {

    // Parse unary expression
    auto unaryexpr = parseUnaryExpr();

    MultExprNode::SubType subtype {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    if (!unaryexpr)
        return nullptr;
    else
        nodes.push_back(move(unaryexpr));


    // Check for optional ("*"|"/")  multiplicative-expression
    auto tk = nextToken();
    auto t = checkForArithmeticToken(tk.get());

    if (t == ArithmeticType::Mul || t == ArithmeticType::Div) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

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

    return make_unique<MultExprNode>(subtype, move(nodes), ref);
}

std::unique_ptr<UnaryExprNode> Parser::parseUnaryExpr() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    vector<unique_ptr<TreeNode>> nodes{};

    UnaryExprNode::SubType subtype{UnaryExprNode::SubType::NoSign};

    auto t = checkForArithmeticToken(currToken.get());

    if (t == ArithmeticType::Plus) {

        subtype = UnaryExprNode::SubType::Plus;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    }
    else if (t == ArithmeticType::Minus) {
        subtype = UnaryExprNode::SubType::Minus;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    }
    else
        lookaheadToken = move(currToken);

    auto primaryexpr = parsePrimaryExpr();

    if (!primaryexpr)
        return nullptr;

    nodes.push_back(move(primaryexpr));

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<UnaryExprNode>(subtype, move(nodes), ref);
}


std::unique_ptr<AssignExprNode> Parser::parseAssignExpr() {

    vector<unique_ptr<TreeNode>> nodes{};

    auto tk = nextToken();

    // Check for identifier
    if (tk->tokentype != TokenType::Identifier) {
        lookaheadToken = move(tk);
        return nullptr;
    }

    nodes.push_back(make_unique<IdentifierNode>(tk->location, static_cast<Identifier*>(tk.get())->id));


    // Check for :=
    tk = nextToken();

    if (!tk)
        return nullptr;


    auto t = checkForArithmeticToken(tk.get());
    if (t != ArithmeticType::VarAssign) {

        manager.printErrorMessage("Error: ':=' expected", tk->location);
        return nullptr;
    }

    nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

    // Check for additive-expression
    auto addexpr = parseAdditiveExpr();

    if (!addexpr)
        return nullptr;

    nodes.push_back(move(addexpr));

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AssignExprNode>(move(nodes), ref);

}


std::unique_ptr<Statement> Parser::parseStatement() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    Statement::SubType subtype{Statement::SubType::Assign};


    // get the current token, either from lookahead of parent method or from lexer
    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    // Check for RETURN
    auto t = checkForKeywordToken(currToken.get());
    if(t == KeywordType::Ret) {

        subtype = Statement::SubType::Return;
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));

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

    return make_unique<Statement>(subtype, move(nodes), ref);
}

std::unique_ptr<StatementList> Parser::parseStatementList() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto statement = parseStatement();

    if (!statement)
        return nullptr;

    nodes.push_back(move(statement));

    auto tk = nextToken();
    auto t = checkForSeparatorToken(tk.get());

    while(t == SeparatorType::SemiColon) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

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

    return make_unique<StatementList>(move(nodes), ref);
}


std::unique_ptr<CompoundStatement> Parser::parseCompoundStatement() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto tk = nextToken();


    // Check for 'BEGIN'
    if (!tk)
        return nullptr;

    auto t = checkForKeywordToken(tk.get());

    if (t == KeywordType::Begin) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));
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

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));
    }
    else {
        manager.printErrorMessage("Error: 'END' expected", tk->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<CompoundStatement>(move(nodes), ref);
}


std::unique_ptr<InitDeclNode> Parser::parseInitDecl() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    if (!currToken)
        return nullptr;

    // vector of child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    // Check for -> Identifier
    if (currToken->tokentype == TokenType::Identifier) {
        nodes.push_back(make_unique<IdentifierNode>(currToken->location, static_cast<Identifier*>(currToken.get())->id));
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
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
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

    return make_unique<InitDeclNode>(std::move(nodes), ref);
}


std::unique_ptr<InitDeclListNode> Parser::parseInitDeclList() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto initdecl = parseInitDecl();

    if (!initdecl)
        return nullptr;

    nodes.push_back(move(initdecl));

    auto tk = nextToken();
    auto t = checkForSeparatorToken(tk.get());

    while(t == SeparatorType::Comma) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

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

    return make_unique<InitDeclListNode>(move(nodes), ref);

}


std::unique_ptr<DeclListNode> Parser::parseDeclList() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    if (!currToken)
        return nullptr;

    // vector of child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    // Check for -> Identifier
    if (currToken->tokentype == TokenType::Identifier) {
        nodes.push_back(make_unique<IdentifierNode>(currToken->location, static_cast<Identifier*>(currToken.get())->id));
    }
    else {
        manager.printErrorMessage("error: Identifier expected", currToken->location);
        return nullptr;
    }



    currToken = nextToken();
    auto t = checkForSeparatorToken(currToken.get());

    while(t == SeparatorType::Comma) {

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));

        currToken = nextToken();

        if (currToken->tokentype == TokenType::Identifier) {
            nodes.push_back(make_unique<IdentifierNode>(currToken->location, static_cast<Identifier*>(currToken.get())->id));
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

    return make_unique<DeclListNode>(move(nodes), ref);

}


std::unique_ptr<ParamDeclNode> Parser::parseParamDecl() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'PARAM'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Parameter)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    else {

        lookaheadToken = move(currToken);
        return nullptr;
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

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<ParamDeclNode>(move(nodes), ref);


}

std::unique_ptr<VarDeclNode> Parser::parseVarDecl() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'VAR'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Var)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    else {
        lookaheadToken = move(currToken);
        return nullptr;
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

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<VarDeclNode>(move(nodes), ref);


}
std::unique_ptr<ConstDeclNode> Parser::parseConstDecl() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto currToken = nextToken();


    // Check for 'CONST'
    if (!currToken)
        return nullptr;

    auto t = checkForKeywordToken(currToken.get());

    if (t == KeywordType::Constant)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    else {
        lookaheadToken = move(currToken);
        return nullptr;
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

        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    }
    else {
        manager.printErrorMessage("Error: ';' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<ConstDeclNode>(move(nodes), ref);


}


std::unique_ptr<FuncDeclNode> Parser::parseFunction() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    bool hasParam{false};
    bool hasVar{false};
    bool hasConst{false};

    auto paramdecl = parseParamDecl();
    if (paramdecl) {
        hasParam = true;
        nodes.push_back(move(paramdecl));
    }

    auto vardecl = parseVarDecl();
    if (vardecl) {
        hasVar = true;
        nodes.push_back(move(vardecl));
    }

    auto constdecl = parseConstDecl();
    if (constdecl) {
        hasConst = true;
        nodes.push_back(move(constdecl));
    }

    auto compstatement = parseCompoundStatement();
    if (!compstatement)
        return nullptr;

    nodes.push_back(move(compstatement));


    // Check for final .
    auto currToken = nextToken();
    auto t = checkForSeparatorToken(currToken.get());
    if (t == SeparatorType::Dot)
        nodes.push_back(make_unique<GenericTerminalNode>(currToken->location));
    else {
        manager.printErrorMessage("error: '.' expected", currToken->location);
        return nullptr;
    }

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};


    return make_unique<FuncDeclNode>(move(nodes), hasParam, hasVar, hasConst, ref);

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
