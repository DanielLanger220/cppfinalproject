#ifndef PLJIT_SEMANTICANALYSER_H
#define PLJIT_SEMANTICANALYSER_H

#include <map>

#include "pljit/Parser/ParseTreeNode.h"
#include "pljit/SemanticAnalysis/AstNode.h"

namespace jit {

class SemanticAnalyser {

    public:

    // Constructor
    SemanticAnalyser(SourceCodeManager& manager, FuncDeclNode& function) : manager{manager}, function{function} {}


    // createTable                  Creates the symbol table by inspecting the declarations of the parameters, variables and constants from the parse tree
    bool createTable();

    // analyseFunction
    std::unique_ptr<AstFunction> analyseFunction();


    private:

    // analyseExpression            Checks, if the expression is a valid arithmetic expression with valid identifiers by recursively checking its sub expressions.
    //                              If successfull, returns an AstArithmeticExpression node.
    std::unique_ptr<AstArithmeticExpression> analyseExpression(const ParseTreeNode& expression);

    // analyseStatement             If the statment is a return statement, checks whether the return value is a valid expression.
    //                              If it is an assignment expression, checks for a valid assignment.
    //                              In both cases, if successfull, returns a AstStatement node.
    std::unique_ptr<AstStatement> analyseStatement(const Statement& statement);

    // analyseIdentifier            Checks, if the identifier was declared.
    //                              If lhs is set to true, also checks if the identifier is a non-constant (the identifier appears on the left hand side of an assignment)
    //                              if lhs is set to false, also checks if the identifier has been initialised (the identifier appears in an arithmetic expression)
    std::unique_ptr<AstIdentifier> analyseIdentifier(const IdentifierNode& id, bool lhs);

    // analyseAssignment            Checks, if the expression on the right hand side is a valid expression and if the identifier on the left hand side is allowed to be assigned to.
    //                              Also updates the hasValue flag of this identifier in the symbol table.
    //                              If successfull, returns an AstAssignment node.
    std::unique_ptr<AstAssignment> analyseAssignment(const AssignExprNode& expr);


    const SourceCodeManager& manager;
    const FuncDeclNode& function;

    SymbolTable table;

    size_t nofidentifiers{0};           // Stores the total number of valid identifiers in the function (parameters + variables + constants)
    size_t nofparameters{0};            // Stores the number of parameters of the function
    size_t nofvariables{0};
    size_t nofconstants{0};

    std::unique_ptr<std::vector<int64_t>> constants{};
};

} // namespace jit

#endif //PLJIT_SEMANTICANALYSER_H
