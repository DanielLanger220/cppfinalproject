#include "SemanticAnalyzer.h"

#include <map>

using namespace std;

namespace jit{

bool SemanticAnalyzer::createTables() {

    table = SymbolTable{};
    nametable.clear();

    // map used to detect multiple declarations of the same identifier
    map<string_view, SourceCodeReference> lookupmap{};

    size_t index{0};

    // Read the Parameters
    if (function.hasParamDecl) {

        const ParamDeclNode& n = static_cast<ParamDeclNode&>(*function.nodes[0]);
        const DeclListNode& decllist = static_cast<DeclListNode&>(*n.nodes[1]);

        // Go through the declaration list (skip all ',')
        for(size_t i = 0; i < decllist.nodes.size(); i += 2)
        {
            const IdentifierNode& identifier = static_cast<IdentifierNode&>(*decllist.nodes[i]);

            // Check, if identifier already exists
            auto res = lookupmap.insert(pair<string_view, SourceCodeReference>(manager.getString(identifier.location), identifier.location));

            if (!res.second) {
                manager.printErrorMessage("error: Parameter already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", res.first->second);
                return false;
            }

            table.insertEntry(index, identifier.location, false, true, 0);
            nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), index));

            ++index;
        }

    }

    // Read the Variables
    if (function.hasVarDecl) {

        size_t varindex = function.hasParamDecl ? 1 : 0;
        const VarDeclNode& n = static_cast<VarDeclNode&>(*function.nodes[varindex]);
        const DeclListNode& decllist = static_cast<DeclListNode&>(*n.nodes[1]);

        // Go through the declaration list (skip all ',')
        for(size_t i = 0; i < decllist.nodes.size(); i += 2)
        {
            const IdentifierNode& identifier = static_cast<IdentifierNode&>(*decllist.nodes[i]);

            // Check, if identifier already exists
            auto res = lookupmap.insert(pair<string_view, SourceCodeReference>(manager.getString(identifier.location), identifier.location));

            if (!res.second) {
                manager.printErrorMessage("error: Variable already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", res.first->second);
                return false;
            }

            table.insertEntry(index, identifier.location, false, false, 0);
            nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), index));

            ++index;
        }


    }

    // Read the Constants
    if (function.hasConstDecl) {

        size_t constindex = (function.hasParamDecl ? 1 :0) + (function.hasVarDecl ? 1 : 0);
        const ConstDeclNode& n = static_cast<ConstDeclNode&>(*function.nodes[constindex]);
        const InitDeclListNode& decllist = static_cast<InitDeclListNode&>(*n.nodes[1]);


        // Go through the declaration list (skip all ',')
        for(size_t i = 0; i < decllist.nodes.size(); i += 2)
        {
            const InitDeclNode& decl = static_cast<InitDeclNode&>(*decllist.nodes[i]);
            const IdentifierNode& identifier = static_cast<IdentifierNode&>(*decl.nodes[0]);
            const LiteralNode& literal = static_cast<LiteralNode&>(*decl.nodes[2]);

            // Check, if identifier already exists
            auto res = lookupmap.insert(pair<string_view, SourceCodeReference>(manager.getString(identifier.location), identifier.location));

            if (!res.second) {
                manager.printErrorMessage("error: Constant already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", res.first->second);
                return false;
            }

            table.insertEntry(index, identifier.location, true, true, literal.value);
            nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), index));


            ++index;
        }


    }


    cout << index << " Identifers erkannt!\n";

    for(auto t : table.table)
    {
        cout << manager.getString(t.declaration) << "\t\t\t" << t.isConst << " " << t.hasValue << " " << t.value << endl;
    }


    for(auto t : nametable)
        cout << t.first << "\t\t\t" << t.second << endl;

    return true;
}



std::unique_ptr<AstIdentifier> SemanticAnalyzer::analyseIdentifier(const IdentifierNode& id, bool lhs) {

    auto res = nametable.find(manager.getString(id.location));

    if (res == nametable.end()) {

        manager.printErrorMessage("error: undeclared identifier", id.location);
        return nullptr;
    }

    size_t index = nametable[manager.getString(id.location)];

    cout << "Check identifier\t" << manager.getString(id.location) << "\t" << lhs << endl;

    // If identifier appears on the left hand side, check if it is non-constant
    if (lhs && table.isConst(index)) {

        manager.printErrorMessage("error: unallowed assignment to constant variable", id.location);
        return nullptr;
    }
    // If identifier appears on the right hand side, check if it is initialised
    else if (!lhs && !table.hasValue(index)) {

        manager.printErrorMessage("error: use of uninitialised variable in expression", id.location);
        return nullptr;
    }

    return make_unique<AstIdentifier>(id.location, index);;
}

std::unique_ptr<AstArithmeticExpression> SemanticAnalyzer::analyzeExpression(const ParseTreeNode& expression) {


    switch(expression.nodetype) {

        case ParseTreeNode::Type::Literal:
            return make_unique<AstLiteral>(expression.location, static_cast<const LiteralNode&>(expression).value);

        case ParseTreeNode::Type::Identifier:
            return analyseIdentifier(static_cast<const IdentifierNode&>(expression), false);

        case ParseTreeNode::Type::PrimaryExpr: {

            const auto& primexpr = static_cast<const PrimaryExprNode&>(expression);

            // Literal
            if (primexpr.subtype == PrimaryExprNode::SubType::Literal)
                return make_unique<AstLiteral>(primexpr.location, static_cast<const LiteralNode&>(*primexpr.nodes[0]).value);
            // Identifier
            else if (primexpr.subtype == PrimaryExprNode::SubType::Identifier)
                return analyseIdentifier(static_cast<const IdentifierNode&>(*primexpr.nodes[0]), false);
            // Arithmetic expression in parentheses
            else
                return analyzeExpression(*primexpr.nodes[1]);
        }
        case ParseTreeNode::Type::UnaryExpr: {

            auto& unaryexpr = static_cast<const UnaryExprNode&>(expression);

            if (unaryexpr.subtype == UnaryExprNode::SubType::Plus)
                return analyzeExpression(*unaryexpr.nodes[1]);
            else if (unaryexpr.subtype == UnaryExprNode::SubType::NoSign)
                return analyzeExpression(*unaryexpr.nodes[0]);
            else {
                auto subexpr = analyzeExpression(*unaryexpr.nodes[1]);
                return make_unique<AstUnaryArithmeticExpression>(unaryexpr.location, move(subexpr));
            }
        }
        case ParseTreeNode::Type::MultExpr: {

            auto& multexpr = static_cast<const MultExprNode&>(expression);

            if (multexpr.subtype == MultExprNode::SubType::Unary)
                return analyzeExpression(*multexpr.nodes[0]);
            else {
                auto lhs = analyzeExpression(*multexpr.nodes[0]);
                auto rhs = analyzeExpression(*multexpr.nodes[2]);
                //TODO: Add arithmetic operations to Generic Terminal node and correct the following line
                return make_unique<AstBinaryArithmeticExpression>(multexpr.location, move(lhs), move(rhs), AstBinaryArithmeticExpression::ArithmeticOperation::Mul);
            }
        }
        case ParseTreeNode::Type::AdditiveExpr: {

            auto& addexpr = static_cast<const AdditiveExprNode&>(expression);

            if (addexpr.subtype == AdditiveExprNode::SubType::Unary)
                return analyzeExpression(*addexpr.nodes[0]);
            else {
                auto lhs = analyzeExpression(*addexpr.nodes[0]);
                auto rhs = analyzeExpression(*addexpr.nodes[2]);
                //TODO: Add arithmetic operations to Generic Terminal node and correct the following line
                return make_unique<AstBinaryArithmeticExpression>(addexpr.location, move(lhs), move(rhs), AstBinaryArithmeticExpression::ArithmeticOperation::Plus);
            }
        }
        default:
            cerr << "Compiler error\n";
            exit(EXIT_FAILURE);
    }

}


std::unique_ptr<AstStatement> SemanticAnalyzer::analyzeStatement(const Statement& statement) {

    // Check, if statement is an assignment or a return statement
    if (statement.subtype == Statement::SubType::Assign)
        return analyzeAssignment(static_cast<const AssignExprNode&>(*statement.nodes[0]));
    else {

        // analyse the additive expression of the return statement
        auto addexpr = analyzeExpression(*statement.nodes[1]);

        if (!addexpr)
            return nullptr;

        return make_unique<AstReturn>(statement.location, move(addexpr));
    }
}

std::unique_ptr<AstAssignment> SemanticAnalyzer::analyzeAssignment(const AssignExprNode& expr) {

    // Analyse the identifier on the left hand side
    auto id = analyseIdentifier(static_cast<const IdentifierNode&>(*expr.nodes[0]), true);

    if (!id)
        return nullptr;

    // Analyse the expression on the right hand side
    auto addexpr = analyzeExpression(static_cast<const AdditiveExprNode&>(*expr.nodes[2]));

    if (!addexpr)
        return nullptr;

    return make_unique<AstAssignment>(expr.location, move(id), move(addexpr));
}

std::unique_ptr<AstFunction> SemanticAnalyzer::analyseFunction(const FuncDeclNode& function) {


    //TODO: Perhaps initialise the tables here

    bool hasreturn{false};

    // Vector to store the statements of the AstFunction object
    vector<unique_ptr<AstStatement>> aststatements{};


    const StatementList* statementlist = function.getStatements();

    if (!statementlist)
        return nullptr;

    for(size_t i = 0; i < statementlist->nodes.size(); i += 2) {

        const Statement& st = static_cast<Statement&>(*statementlist->nodes[i]);

        auto aststatement = analyzeStatement(st);

        if(!aststatement)
            return nullptr;

        if (aststatement->subtype == AstStatement::SubType::AstReturn)
            hasreturn = true;


        aststatements.push_back(move(aststatement));

    }

    if (!hasreturn) {
        cerr << "error: missing RETURN statement in function\n";
        return nullptr;
    }

    return make_unique<AstFunction>(move(aststatements));
}



} // namespace jit