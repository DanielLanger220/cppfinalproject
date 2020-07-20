#include "Parser.h"

namespace jit {

using namespace std;


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
    else if (checkForOpenPar(currToken.get())) {

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

        if (n && checkForClosePar(n.get())) {
            nodes.push_back(make_unique<GenericTerminalNode>(n->location));

            size_t range = manager.getabsolutePosition(n->location) + 1 - manager.getabsolutePosition(nodes.front()->location);
            SourceCodeReference ref{nodes.front()->location, range};
            return make_unique<PrimaryExprNode>(PrimaryExprNode::SubType::AdditiveExpr, move(nodes), ref);

        }
        else {
            manager.printErrorMessage("Error: ')' expected", n->location);
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

    AdditiveExprNode::SubType t {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    if (!multexpr)
        return nullptr;
    else
        nodes.push_back(move(multexpr));


    // Check for optional ("+"|"-")  additive-expression
    auto tk = nextToken();
    if (tk && checkForAddSub(tk.get())) {
        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

        auto addexpr = parseAdditiveExpr();

        if (!addexpr)
            return nullptr;

        t = AdditiveExprNode::SubType::Binary;
        nodes.push_back(move(addexpr));

    }
    else
        lookaheadToken = move(tk);


    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AdditiveExprNode>(t, move(nodes), ref);

}


std::unique_ptr<MultExprNode> Parser::parseMultExpr() {

    // Parse unary expression
    auto unaryexpr = parseUnaryExpr();

    MultExprNode::SubType t {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    if (!unaryexpr)
        return nullptr;
    else
        nodes.push_back(move(unaryexpr));


    // Check for optional ("*"|"/")  multiplicative-expression
    auto tk = nextToken();

    if (tk && checkForMulDiv(tk.get())) {
        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

        auto multexpr = parseMultExpr();

        if (!multexpr)
            return nullptr;

        t = MultExprNode::SubType::Binary;
        nodes.push_back(move(multexpr));

    }
    else
        lookaheadToken = move(tk);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<MultExprNode>(t, move(nodes), ref);
}

std::unique_ptr<UnaryExprNode> Parser::parseUnaryExpr() {

    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<Token> currToken = nextToken();

    vector<unique_ptr<TreeNode>> nodes{};

    bool sign{false};

    if (checkForAddSub(currToken.get())) {

        sign = true;
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

    return make_unique<UnaryExprNode>(sign, move(nodes), ref);
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


    if (!checkForVarAssign(tk.get())) {

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

    Statement::SubType t{Statement::SubType::Assign};


    // get the current token, either from lookahead of parent method or from lexer
    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    // Check for RETURN
    if(checkForReturn(currToken.get())) {

        t = Statement::SubType::Return;
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

    return make_unique<Statement>(t, move(nodes), ref);
}

std::unique_ptr<StatementList> Parser::parseStatementList() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto statement = parseStatement();

    if (!statement)
        return nullptr;

    nodes.push_back(move(statement));

    auto tk = nextToken();

    while(tk && checkForSemiColon(tk.get())) {

        nodes.push_back(make_unique<GenericTerminalNode>(tk->location));

        statement = parseStatement();

        if(statement)
            nodes.push_back(move(statement));
        else {
            return nullptr;
        }
        tk = nextToken();
    }

    // move the falsely read token that was no ';' back to the lookahead
    lookaheadToken = move(tk);

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<StatementList>(move(nodes), ref);
}


std::unique_ptr<CompoundStatement> Parser::parseCompundStatement() {

    // Vector for the child nodes
    vector<unique_ptr<TreeNode>> nodes{};

    auto tk = nextToken();


    // Check for 'BEGIN'
    if (!tk)
        return nullptr;

    if (checkForBegin(tk.get())) {

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

    if (checkForEnd(tk.get())) {

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


// **********************************************
// Helper methods
// **********************************************

bool Parser::checkForAddSub(const Token* tk) {

    if (tk->tokentype != TokenType::ArithmeticOperator)
        return false;

    auto t = static_cast<const ArithmeticOperator*>(tk)->arithmetictype;

    return t == ArithmeticType::Plus || t == ArithmeticType::Minus;
}

bool Parser::checkForMulDiv(const Token* tk) {

    if (tk->tokentype != TokenType::ArithmeticOperator)
        return false;

    auto t = static_cast<const ArithmeticOperator*>(tk)->arithmetictype;

    return t == ArithmeticType::Mul || t == ArithmeticType::Div;
}

bool Parser::checkForOpenPar(const Token* tk) {

    if (tk->tokentype != TokenType::Separator)
        return false;

    return (static_cast<const Separator*>(tk)->separatortype == SeparatorType::OpenPar);

}

bool Parser::checkForClosePar(const Token *tk) {

    if (tk->tokentype != TokenType::Separator)
        return false;

    return (static_cast<const Separator*>(tk)->separatortype == SeparatorType::ClosePar);

}
bool Parser::checkForVarAssign(const Token* tk) {

    if (tk->tokentype != TokenType::ArithmeticOperator)
        return false;

    return (static_cast<const ArithmeticOperator*>(tk)->arithmetictype == ArithmeticType::VarAssign);
}

bool Parser::checkForReturn(const Token* tk) {

    if (tk->tokentype != TokenType::Keyword)
        return false;

    return (static_cast<const Keyword*>(tk)->keywordtype == KeywordType::Ret);

}

bool Parser::checkForSemiColon(const Token* tk) {

    if (tk->tokentype != TokenType::Separator)
        return false;

    return (static_cast<const Separator*>(tk)->separatortype == SeparatorType::SemiColon);

}

bool Parser::checkForBegin(const Token* tk) {

    if (tk->tokentype != TokenType::Keyword)
        return false;

    return (static_cast<const Keyword*>(tk)->keywordtype == KeywordType::Begin);

}

bool Parser::checkForEnd(const Token* tk) {

    if (tk->tokentype != TokenType::Keyword)
        return false;

    return (static_cast<const Keyword*>(tk)->keywordtype == KeywordType::End);

}

} // namespace jit
