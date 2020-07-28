#ifndef PLJIT_SEMANTICANALYZER_H
#define PLJIT_SEMANTICANALYZER_H

#include <map>

#include "pljit/Parser/ParseTreeNode.h"
#include "pljit/SemanticAnalysis/AstNode.h"

namespace jit {

class SemanticAnalyzer {

    public:


    SemanticAnalyzer(SourceCodeManager& manager, FuncDeclNode& function) : manager{manager}, function{function} {}

    bool createTables();
    std::unique_ptr<AstFunction> analyseFunction();

    private:

    std::unique_ptr<AstArithmeticExpression> analyzeExpression(const ParseTreeNode& expression);
    std::unique_ptr<AstStatement> analyzeStatement(const Statement& statement);

    // analyseIdentifier            Checks, if the identifier was declared.
    //                              If lhs is set to true, also checks if the identifier is a non-constant (the identifier appears on the left hand side of an assignment)
    //                              if lhs is set to false, also checks if the identifier has been initialised (the identifier appears in an arithmetic expression)
    std::unique_ptr<AstIdentifier> analyseIdentifier(const IdentifierNode& id, bool lhs);
    std::unique_ptr<AstAssignment> analyzeAssignment(const AssignExprNode& expr);


    const SourceCodeManager& manager;
    const FuncDeclNode& function;

    SymbolTable table;
    std::map<std::string_view, size_t> nametable{};


};

} // namespace jit

#endif //PLJIT_SEMANTICANALYZER_H
