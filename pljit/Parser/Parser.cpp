#include "pljit/Parser/Parser.h"


using namespace std;

namespace jit {

using TokenType = Token::TokenType;
using SeparatorType = Separator::SeparatorType;
using ArithmeticType = ArithmeticOperator::ArithmeticType;
using KeywordType = Keyword::KeywordType;


unique_ptr<GenericTerminalNode> Parser::parseSeparator(SeparatorType t, bool mandatory) {

    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype != Token::TokenType::Separator || static_cast<Separator*>(currToken.get())->separatortype != t) {

        if (mandatory)
            manager.printErrorMessage("error: '" + Separator::toString(t) + "' expected", currToken->location);

        // Move falsely read token back to the lookahead place
        lookaheadToken = move(currToken);

        return nullptr;
    }

    return make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other);
}

unique_ptr<GenericTerminalNode> Parser::parseKeyword(KeywordType t, bool mandatory)
{
    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype != Token::TokenType::Keyword || static_cast<Keyword*>(currToken.get())->keywordtype != t)  {

        if (mandatory)
            manager.printErrorMessage("error: '" + Keyword::toString(t) + "' expected", currToken->location);

        // Move falsely read token back to the lookahead place
        lookaheadToken = move(currToken);

        return nullptr;
    }

    return make_unique<GenericTerminalNode>(currToken->location, GenericTerminalNode::SubType::Other);
}

unique_ptr<GenericTerminalNode> Parser::parseArithmeticOperator(ArithmeticType t, bool mandatory)
{
    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype != Token::TokenType::ArithmeticOperator || static_cast<ArithmeticOperator*>(currToken.get())->arithmetictype != t) {

        if (mandatory)
            manager.printErrorMessage("error: '" + ArithmeticOperator::toString(t) + "' expected", currToken->location);

        // Move falsely read token back to the lookahead place
        lookaheadToken = move(currToken);

        return nullptr;
    }

    GenericTerminalNode::SubType generictype{};

    switch(t) {
        case ArithmeticType::Plus:
            generictype = GenericTerminalNode::SubType::Plus;
            break;
        case ArithmeticType::Minus:
            generictype = GenericTerminalNode::SubType::Minus;
            break;
        case ArithmeticType::Mul:
            generictype = GenericTerminalNode::SubType::Mul;
            break;
        case ArithmeticType::Div:
            generictype = GenericTerminalNode::SubType::Div;
            break;
        default:
            generictype = GenericTerminalNode::SubType::Other;
    }

    return make_unique<GenericTerminalNode>(currToken->location, generictype);
}

unique_ptr<IdentifierNode> Parser::parseIdentifier(bool mandatory) {

    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype == TokenType::Identifier)
        return make_unique<IdentifierNode>(currToken->location);
    else {
        if (mandatory)
            manager.printErrorMessage("error: identifier expected", currToken->location);

        // Move falsely read token back to the lookahead place
        lookaheadToken = move(currToken);
        return nullptr;
    }
}

unique_ptr<LiteralNode> Parser::parseLiteral(bool mandatory) {

    auto currToken = nextToken();

    if (!currToken)
        return nullptr;

    if (currToken->tokentype == TokenType::Literal)
        return make_unique<LiteralNode>(currToken->location, static_cast<Literal*>(currToken.get())->value);
    else {
        if(mandatory)
            manager.printErrorMessage("error: literal expected", currToken->location);

        // Move falsely read token back to the lookahead place
        lookaheadToken = move(currToken);
        return nullptr;
    }

}

unique_ptr<PrimaryExprNode> Parser::parsePrimaryExpr(bool mandatory) {


    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    // Check for -> Identifier alternative
    unique_ptr<ParseTreeNode> n;
    if ((n = parseIdentifier(false))) {
        nodes.push_back(move(n));   // Push identifier to the nodes
        return make_unique<PrimaryExprNode>(nodes[0]->location, move(nodes), PrimaryExprNode::SubType::Identifier);
    }

    // Check for -> Literal alternative
    if ((n = parseLiteral(false))) {
        nodes.push_back(move(n));   // Push literal to the nodes
        return make_unique<PrimaryExprNode>(nodes[0]->location, move(nodes), PrimaryExprNode::SubType::Literal);
    }


    // Check for -> "("  additive-expr  ")" alternative

    // Check for '('
    if((n = parseSeparator(SeparatorType::OpenPar, false))) {

         // Push "(" to the child nodes
        nodes.push_back(move(n));

        // Parse the additive expression (mandatory expression because of the open '(' )
        if (!(n = parseAdditiveExpr(true)))
            return nullptr;

        // Push the additive expression to the child nodes
       nodes.push_back(move(n));

        // Check for ")"
        n = parseSeparator(SeparatorType::ClosePar, true);

        if (n) {
            size_t range = manager.getabsolutePosition(n->location) + 1 - manager.getabsolutePosition(nodes.front()->location);
            SourceCodeReference ref{nodes.front()->location, range};

            // Push the ')' to the child nodes
            nodes.push_back(move(n));

            return make_unique<PrimaryExprNode>(ref, move(nodes), PrimaryExprNode::SubType::AdditiveExpr);

        }
        else {
            manager.printErrorMessage("... to match this '('", nodes[0]->location);
            return nullptr;
        }
    }
    else { // No primary expression could be parsed

        if (mandatory)
            manager.printErrorMessage("error: Unexpected Token", (lookaheadToken ? lookaheadToken->location : lex.refToCurrentPosition()));

        return nullptr;
    }
}


unique_ptr<AdditiveExprNode> Parser::parseAdditiveExpr(bool mandatory) {


    AdditiveExprNode::SubType subtype {AdditiveExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Parse multiplicative expression
    if (!(n = parseMultExpr(mandatory)))
        return nullptr;

    // Push multiplicative expression to the child nodes
    nodes.push_back(move(n));

    // Check or optional (+|-) add-expr
    if ((n = parseArithmeticOperator(ArithmeticType::Plus)) || (n = parseArithmeticOperator(ArithmeticType::Minus))) {

        // push the '+' or '-' to the child nodes
        nodes.push_back(move(n));

        // parse the following mandatory additive expression
        if (!(n = parseAdditiveExpr(true)))
            return nullptr;

        // push the additive expression to the child vector
        nodes.push_back(move(n));

        // the additive expression is binary (... (+|-) ...)
        subtype = AdditiveExprNode::SubType::Binary;
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AdditiveExprNode>(ref, move(nodes), subtype) ;
}


unique_ptr<MultExprNode> Parser::parseMultExpr(bool mandatory) {


    MultExprNode::SubType subtype {MultExprNode::SubType::Unary};

    // Vector for child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Parse unary expression
    if (!(n = parseUnaryExpr(mandatory)))
        return nullptr;

    nodes.push_back(move(n));

    // Check for optional (*|/) mult-expr
    if((n = parseArithmeticOperator(ArithmeticType::Mul)) || (n = parseArithmeticOperator(ArithmeticType::Div))) {

        // push the '*' or '/' to the child vector
        nodes.push_back(move(n));

        if (!(n = parseMultExpr(true)))
            return nullptr;

        // push the multiplicative expression to the child vector
        nodes.push_back(move(n));

        // the multiplicative expression is binary (... (*|/) ...)
        subtype = MultExprNode::SubType::Binary;

    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<MultExprNode>(ref, move(nodes), subtype);
}

unique_ptr<UnaryExprNode> Parser::parseUnaryExpr(bool mandatory) {

    vector<unique_ptr<ParseTreeNode>> nodes{};

    UnaryExprNode::SubType subtype{UnaryExprNode::SubType::NoSign};

    // Check for optional + and -
    unique_ptr<ParseTreeNode> n;

    if ((n = parseArithmeticOperator(ArithmeticType ::Plus, false))) {
        subtype = UnaryExprNode::SubType::Plus;
        nodes.push_back(move(n));
    }
    else if((n = parseArithmeticOperator(ArithmeticType::Minus, false))) {
        subtype = UnaryExprNode::SubType::Minus;
        nodes.push_back(move(n));
    }


    // Parse the primary expression (expression is mandatory, if +/- was parsed, otherwise it depends on mandatory flag)
    if (subtype == UnaryExprNode::SubType::NoSign)
        n = parsePrimaryExpr(mandatory);
    else
        n = parsePrimaryExpr(true);

    if (!n)
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<UnaryExprNode>(ref, move(nodes), subtype);
}


unique_ptr<AssignExprNode> Parser::parseAssignExpr(bool mandatory) {

    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Check for identifier
    if (!(n = parseIdentifier(mandatory)))
        return nullptr;

    nodes.push_back(move(n));

    // Check for := (mandatory)
    if (!(n = parseArithmeticOperator(ArithmeticType::VarAssign, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Check for additive-expression (mandatory)
    if (!(n  = parseAdditiveExpr(true)))
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<AssignExprNode>(ref, move(nodes));

}


unique_ptr<Statement> Parser::parseStatement() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    Statement::SubType subtype{Statement::SubType::Assign};

    unique_ptr<ParseTreeNode> n;

    // Check for RETURN
    if((n = parseKeyword(KeywordType::Ret, false))) {

        subtype = Statement::SubType::Return;
        nodes.push_back(move(n));

        // Check for additive-expression
        if(!(n = parseAdditiveExpr(true)))
            return nullptr;

        nodes.push_back(move(n));
    }
    // Check for assignment expression
    else {

        if(!(n = parseAssignExpr(true)))
            return nullptr;

        nodes.push_back(move(n));
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<Statement>(ref, move(nodes), subtype);
}

unique_ptr<StatementList> Parser::parseStatementList() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Parse the first mandatory statement
    if (!(n = parseStatement()))
        return nullptr;

    nodes.push_back(move(n));

    // parse the optional arbitrary many following statements (separated by ;)
    while((n = parseSeparator(SeparatorType::SemiColon, false))) {

        nodes.push_back(move(n));

        if (!(n = parseStatement()))
            return nullptr;

        nodes.push_back(move(n));
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<StatementList>(ref, move(nodes));
}


unique_ptr<CompoundStatement> Parser::parseCompoundStatement() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Check for 'BEGIN'
    if (!(n = parseKeyword(KeywordType::Begin, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the statement list
    if (!(n = parseStatementList()))
        return nullptr;

    nodes.push_back(move(n));

    // Check for 'END'
    if (!(n = parseKeyword(KeywordType::End, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<CompoundStatement>(ref, move(nodes));
}


unique_ptr<InitDeclNode> Parser::parseInitDecl() {

    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};


    // get the current token, either from lookahead of parent method or from lexer
    unique_ptr<ParseTreeNode> n;


    // Check for identifier
    if (!(n = parseIdentifier(true)))
        return nullptr;

    nodes.push_back(move(n));

    // Check for '='
    if (!(n = parseArithmeticOperator(ArithmeticType::Assign, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Check for literal
    if (!(n = parseLiteral(true)))
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<InitDeclNode>(ref, move(nodes));
}


unique_ptr<InitDeclListNode> Parser::parseInitDeclList() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Parse the first mandatory constant initialisation
    if (!(n = parseInitDecl()))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the optional arbitrary many following constant initialisations (separated by ',')
    while((n = parseSeparator(SeparatorType::Comma, false))) {

        nodes.push_back(move(n));

        if(!(n = parseInitDecl()))
            return nullptr;

        nodes.push_back(move(n));
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<InitDeclListNode>(ref, move(nodes));
}

unique_ptr<DeclListNode> Parser::parseDeclList() {

    // vector of child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Parse the first mandatory identifier
    if (!(n = parseIdentifier(true)))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the optional arbitrary many following declarations (separated by ',')
    while((n = parseSeparator(SeparatorType::Comma, false))) {

        nodes.push_back(move(n));

        if (!(n = parseIdentifier(true)))
            return nullptr;

        nodes.push_back(move(n));
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<DeclListNode>(ref, move(nodes));
}


optional<unique_ptr<ParamDeclNode>> Parser::parseParamDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Check for 'PARAM'
    if (!(n = parseKeyword(KeywordType::Parameter, false)))
        return nullopt;

    nodes.push_back(move(n));


    // Parse the decl-list
    if (!(n = parseDeclList()))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the final ';'
    if (!(n = parseSeparator(SeparatorType::SemiColon, true)))
        return nullptr;

    nodes.push_back(move(n));

    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    return make_unique<ParamDeclNode>(ref, move(nodes));
}

optional<unique_ptr<VarDeclNode>> Parser::parseVarDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Check for 'VAR'
    if (!(n = parseKeyword(KeywordType::Var, false)))
        return nullopt;

    nodes.push_back(move(n));


    // Parse the decl-list
    if (!(n = parseDeclList()))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the final ';'
    if (!(n = parseSeparator(SeparatorType::SemiColon, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<VarDeclNode>(ref, move(nodes));
}

optional<unique_ptr<ConstDeclNode>> Parser::parseConstDecl() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    unique_ptr<ParseTreeNode> n;

    // Check for 'CONST'
    if (!(n = parseKeyword(KeywordType::Constant, false)))
        return nullopt;

    nodes.push_back(move(n));


    // Parse the decl-list
    if (!(n = parseInitDeclList()))
        return nullptr;

    nodes.push_back(move(n));

    // Parse the final ';'
    if (!(n = parseSeparator(SeparatorType::SemiColon, true)))
        return nullptr;

    nodes.push_back(move(n));

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<ConstDeclNode>(ref, move(nodes));
}


unique_ptr<FuncDeclNode> Parser::parseFunction() {

    // Vector for the child nodes
    vector<unique_ptr<ParseTreeNode>> nodes{};

    bool hasParam{false};
    bool hasVar{false};
    bool hasConst{false};

    auto paramdecl = parseParamDecl();
    if (paramdecl) {
        if (!paramdecl.value())
            return nullptr;

        hasParam = true;
        nodes.push_back(move(paramdecl.value()));
    }

    auto vardecl = parseVarDecl();
    if (vardecl) {
        if(!vardecl.value())
            return nullptr;

        hasVar = true;
        nodes.push_back(move(vardecl.value()));
    }

    auto constdecl = parseConstDecl();
    if (constdecl) {
        if(!constdecl.value())
            return nullptr;

        hasConst = true;
        nodes.push_back(move(constdecl.value()));
    }

    auto compstatement = parseCompoundStatement();
    if (!compstatement)
        return nullptr;

    nodes.push_back(move(compstatement));

    auto n = parseSeparator(SeparatorType::Dot, true);
    if (!n)
        return nullptr;

    nodes.push_back(move(n));

    // Correct function was parsed, now check if end of file is reached.
    if (!lex.checkForEndOfFile()) {
        manager.printErrorMessage("error: Unexpected Tokens", lex.refToCurrentPosition());
        return nullptr;
    }

    // Determine the range ( resp. the length) of the node in the source code and create a source code reference
    size_t range = manager.getabsolutePosition(nodes.back()->location) + nodes.back()->location.range - manager.getabsolutePosition(nodes.front()->location);
    SourceCodeReference ref{nodes.front()->location, range};

    return make_unique<FuncDeclNode>(ref, move(nodes), hasParam, hasVar, hasConst);
}

} // namespace jit
