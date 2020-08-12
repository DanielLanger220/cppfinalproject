#ifndef PLJIT_PARSEPRINTVISITOR_H
#define PLJIT_PARSEPRINTVISITOR_H

#include <fstream>

#include "ParseTreeNode.h"
#include "ParseTreeVisitor.h"
#include "pljit/CodeManagement/SourceCodeManager.h"

namespace jit {

class ParsePrintVisitor : public ParseTreeVisitor {

    public:

    // Constructor          Initializes an object with a specified filename for the output and a corresponding SourceCodeManager
    ParsePrintVisitor(std::string filename, const SourceCodeManager& manager) :  filename{move(filename)}, manager{manager}{}

    // printTree            Prints the parse tree defined by the given node a root
    void printTree(const ParseTreeNode& root);

    // The visit methods for the visitor pattern
    void visit(const IdentifierNode& node) override ;
    void visit(const LiteralNode& node) override;
    void visit(const GenericTerminalNode& node) override;
    void visit(const PrimaryExprNode& node) override;
    void visit(const UnaryExprNode& node) override;
    void visit(const MultExprNode& node) override;
    void visit(const AdditiveExprNode& node) override;
    void visit(const AssignExprNode& node) override;
    void visit(const Statement& node) override;
    void visit(const StatementList& node) override;
    void visit(const CompoundStatement& node) override;
    void visit(const ParamDeclNode& node) override;
    void visit(const VarDeclNode& node) override;
    void visit(const ConstDeclNode& node) override;
    void visit(const DeclListNode& node) override;
    void visit(const InitDeclNode& node) override;
    void visit(const InitDeclListNode& node) override;
    void visit(const FuncDeclNode& node) override;

    private:

    size_t index{0};                    // Index used for numbering the nodes
    std::ofstream of{};                 // The output stream to write with
    const std::string filename{};       // The name of the output file
    const SourceCodeManager& manager;   // The corresponding SourceCodeManager
    std::vector<size_t> indexstack{};   // Stack that is used for a correct indexing of the nodes

    // Helper methods

    // printNonTerminalNode             Prints the given non terminal node and all children
    void printNonTerminalNode(const std::string& label, const NonTerminalTreeNode& node);

};

} // namespace jit


#endif //PLJIT_PARSEPRINTVISITOR_H
