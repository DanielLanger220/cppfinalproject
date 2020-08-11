#include "SemanticAnalyser.h"

#include <map>

using namespace std;

namespace jit{


bool SemanticAnalyser::createTable() {

    table = SymbolTable{};

    constantTable.clear();
    nametable.clear();

    nofidentifiers = 0;
    nofparameters = 0;
    nofvariables = 0;


    // Read the Parameters
    if (function.hasParamDecl) {

        const ParamDeclNode& n = static_cast<ParamDeclNode&>(*function.nodes[0]);
        const DeclListNode& decllist = static_cast<DeclListNode&>(*n.nodes[1]);

        // Go through the declaration list (skip all ',')
        for(size_t i = 0; i < decllist.nodes.size(); i += 2)
        {
            const IdentifierNode& identifier = static_cast<IdentifierNode&>(*decllist.nodes[i]);

            // Try to insert the name of the identifier into the nametable
            auto res = nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), nofidentifiers));

            // Check, if identifier already exists
            if (!res.second)
            {
                manager.printErrorMessage("error: Parameter already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", table.table[res.first->second].declaration);
                return false;
            }

            table.insertEntry(identifier.location, false, true);

            ++nofidentifiers;
        }

        nofparameters = nofidentifiers;
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

            // Try to insert the name of the identifier into the nametable
            auto res = nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), nofidentifiers));

            // Check, if identifier already exists
            if (!res.second)
            {
                manager.printErrorMessage("error: Variable already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", table.table[res.first->second].declaration);
                return false;
            }

            table.insertEntry(identifier.location, false, false);

            ++nofidentifiers;
            ++nofvariables;
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

            // Try to insert the name of the identifier into the nametable
            auto res = nametable.insert(pair<string_view, size_t>(manager.getString(identifier.location), nofidentifiers));

            // Check, if identifier already exists
            if (!res.second)
            {
                manager.printErrorMessage("error: Constant already declared ...", identifier.location);
                manager.printErrorMessage("... first declared here", table.table[res.first->second].declaration);
                return false;
            }

            table.insertEntry(identifier.location, true, true);

            // Add the constant to the vector of constants
            constantTable.push_back(literal.value);

            ++nofidentifiers;
        }
    }

    return true;
}


unique_ptr<AstArithmeticExpression> SemanticAnalyser::analyseIdentifier(const IdentifierNode& id, bool lhs) {

    auto res = nametable.find(manager.getString(id.location));

    if (res == nametable.end()) {
        manager.printErrorMessage("error: undeclared identifier", id.location);
        return nullptr;
    }

    size_t index = res->second;

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

    // If the identifier is a constant, create a literal node, otherwise create an identifier node
    if (table.isConst(index))
        return make_unique<AstLiteral>(id.location, constantTable[index - nofparameters - nofvariables]);
    else
        return make_unique<AstIdentifier>(id.location, index);

}

unique_ptr<AstArithmeticExpression> SemanticAnalyser::analyseExpression(const ParseTreeNode& expression) {

    switch(expression.nodetype) {

        case ParseTreeNode::Type::Literal:
            return make_unique<AstLiteral>(expression.location, static_cast<const LiteralNode&>(expression).value);

        case ParseTreeNode::Type::Identifier:
            return analyseIdentifier(static_cast<const IdentifierNode&>(expression),false);

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
                return analyseExpression(*primexpr.nodes[1]);
        }
        case ParseTreeNode::Type::UnaryExpr: {

            auto& unaryexpr = static_cast<const UnaryExprNode&>(expression);

            if (unaryexpr.subtype == UnaryExprNode::SubType::Plus)
                return analyseExpression(*unaryexpr.nodes[1]);
            else if (unaryexpr.subtype == UnaryExprNode::SubType::NoSign)
                return analyseExpression(*unaryexpr.nodes[0]);
            else {
                auto subexpr = analyseExpression(*unaryexpr.nodes[1]);
                if (!subexpr)
                    return nullptr;

                return make_unique<AstUnaryArithmeticExpression>(unaryexpr.location, move(subexpr));
            }
        }
        case ParseTreeNode::Type::MultExpr: {

            auto& multexpr = static_cast<const MultExprNode&>(expression);

            if (multexpr.subtype == MultExprNode::SubType::Unary)
                return analyseExpression(*multexpr.nodes[0]);
            else {
                auto lhs = analyseExpression(*multexpr.nodes[0]);
                if (!lhs)
                    return nullptr;

                auto rhs = analyseExpression(*multexpr.nodes[2]);
                if (!rhs)
                    return nullptr;

                auto op = static_cast<GenericTerminalNode&>(*multexpr.nodes[1]).subtype;

                assert(op == GenericTerminalNode::SubType::Mul || op == GenericTerminalNode::SubType::Div);

                auto astop = AstBinaryArithmeticExpression::ArithmeticOperation::Mul;

                if (op == GenericTerminalNode::SubType::Div)
                    astop = AstBinaryArithmeticExpression::ArithmeticOperation::Div;

                return make_unique<AstBinaryArithmeticExpression>(multexpr.location, move(lhs), move(rhs), astop);
            }
        }
        case ParseTreeNode::Type::AdditiveExpr: {

            auto& addexpr = static_cast<const AdditiveExprNode&>(expression);

            if (addexpr.subtype == AdditiveExprNode::SubType::Unary)
                return analyseExpression(*addexpr.nodes[0]);
            else {
                auto lhs = analyseExpression(*addexpr.nodes[0]);
                if (!lhs)
                    return nullptr;

                auto rhs = analyseExpression(*addexpr.nodes[2]);
                if(!rhs)
                    return nullptr;


                auto op = static_cast<GenericTerminalNode&>(*addexpr.nodes[1]).subtype;

                assert(op == GenericTerminalNode::SubType::Plus || op == GenericTerminalNode::SubType::Minus);

                auto astop = AstBinaryArithmeticExpression::ArithmeticOperation::Plus;

                if (op == GenericTerminalNode::SubType::Minus)
                    astop = AstBinaryArithmeticExpression::ArithmeticOperation::Minus;

                return make_unique<AstBinaryArithmeticExpression>(addexpr.location, move(lhs), move(rhs), astop);
            }
        }
        default:
            cerr << "Compiler error\n";
            exit(EXIT_FAILURE);
    }

}


unique_ptr<AstStatement> SemanticAnalyser::analyseStatement(const Statement& statement) {

    // Check, if statement is an assignment or a return statement
    if (statement.subtype == Statement::SubType::Assign)
        return analyseAssignment(static_cast<const AssignExprNode&>(*statement.nodes[0]));
    else {

        // analyse the additive expression of the return statement
        auto addexpr = analyseExpression(*statement.nodes[1]);

        if (!addexpr)
            return nullptr;

        return make_unique<AstReturn>(statement.location, move(addexpr));
    }
}

unique_ptr<AstAssignment> SemanticAnalyser::analyseAssignment(const AssignExprNode& expr) {

    const IdentifierNode& identifier = static_cast<const IdentifierNode&>(*expr.nodes[0]);

    // Analyse the identifier on the left hand side
    auto id = analyseIdentifier(identifier, true);

    if (!id)
        return nullptr;

    // If analyseIdentifier(.., true) returns a valid pointer, it must always point to an AstIdentifier node
    assert(id->subtype == AstArithmeticExpression::Subtype::Identifier);


    // Analyse the expression on the right hand side
    auto addexpr = analyseExpression(static_cast<const AdditiveExprNode&>(*expr.nodes[2]));

    if (!addexpr)
        return nullptr;


    // Update the symbol table (identifier on the left hand side is now initialised)
    table.table[static_cast<AstIdentifier&>(*id).index].hasValue = true;


    return make_unique<AstAssignment>(expr.location, move(id), move(addexpr));
}

unique_ptr<AstFunction> SemanticAnalyser::analyseFunction() {


    // Initialise the symbol table
    if (!createTable())
        return nullptr;

    bool hasreturn{false};

    // Vector to store the statements of the AstFunction object
    vector<unique_ptr<AstStatement>> aststatements{};

    const StatementList* statementlist = function.getStatements();

    if (!statementlist)
        return nullptr;

    for(size_t i = 0; i < statementlist->nodes.size(); i += 2) {

        const Statement& st = static_cast<Statement&>(*statementlist->nodes[i]);

        auto aststatement = analyseStatement(st);

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

    auto stlist = make_unique<AstStatementList>(SourceCodeReference{aststatements.front()->location}, move(aststatements));


    SourceCodeReference ref = SourceCodeReference{function.nodes.front()->location};
    return make_unique<AstFunction>(ref, move(stlist), nofparameters, nofvariables);
}

} // namespace jit