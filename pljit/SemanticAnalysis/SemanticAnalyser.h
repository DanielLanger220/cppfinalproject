#ifndef PLJIT_SEMANTICANALYSER_H
#define PLJIT_SEMANTICANALYSER_H

#include <map>

#include "pljit/Parser/ParseTreeNode.h"
#include "pljit/SemanticAnalysis/AstNode.h"

namespace jit {

// SemanticAnalyser                     Performs the syntactical analysis on a parse tree and create an Ast
class SemanticAnalyser {

    public:

    // Constructor
    SemanticAnalyser(const SourceCodeManager& manager, const FuncDeclNode& function) : manager{manager}, function{function} {}


    // analyseFunction              Semantically analyses the parsed function and, if all is ok, returns an AstFunction object
    std::unique_ptr<AstFunction> analyseFunction();


    private:

    // createTable                  Creates the symbol table by inspecting the declarations of the parameters, variables and constants from the parse tree
    bool createTable();

    // analyseExpression            Checks, if the expression is a valid arithmetic expression with valid identifiers by recursively checking its sub expressions.
    //                              If successfull, returns an AstArithmeticExpression node.
    std::unique_ptr<AstArithmeticExpression> analyseExpression(const ParseTreeNode& expression);

    // analyseStatement             If the statment is a return statement, checks whether the return value is a valid expression.
    //                              If it is an assignment expression, checks for a valid assignment.
    //                              In both cases, if successfull, returns a AstStatement node.
    std::unique_ptr<AstStatement> analyseStatement(const Statement& statement);


    // analyseIdentifier            Checks, if the identifier was declared. If all checks are ok, returns an AstIdentifier node
    //                              If lhs is set to true, also checks if the identifier is a non-constant (the identifier appears on the left hand side of an assignment)
    //                              if lhs is set to false, also checks if the identifier has been initialised (the identifier appears in an arithmetic expression)
    std::unique_ptr<AstArithmeticExpression> analyseIdentifier(const IdentifierNode& id, bool lhs);

    // analyseAssignment            Checks, if the expression on the right hand side is a valid expression and if the identifier on the left hand side is allowed to be assigned to.
    //                              Also updates the hasValue flag of this identifier in the symbol table.
    //                              If successfull, returns an AstAssignment node.
    std::unique_ptr<AstAssignment> analyseAssignment(const AssignExprNode& expr);


    const SourceCodeManager& manager;       // Reference to the associated source code manager
    const FuncDeclNode& function;           // The given parse tree of the function

    SymbolTable table;                      // Symbol table

    // The number of valid identifiers (parameters + variables + constants), parameters and variables
    // CAUTION:
    // During the semantical analyisis, constants are still counted as identifiers whereas for the final AstFunction object they will be removed
    // and thus not counted anymore, i.e. SemanticAnalyser.nofidentifiers != AstFunction.nofidentifiers
    size_t nofidentifiers{0};
    size_t nofparameters{0};
    size_t nofvariables{0};

    std::map<std::string_view, size_t> nametable{};     // A map to create indices for the identifiers
    std::vector<int64_t> constantTable{};               // Vector to store the values of constants during the semantical analysis
};

} // namespace jit

#endif //PLJIT_SEMANTICANALYSER_H
