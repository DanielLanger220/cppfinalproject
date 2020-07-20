#ifndef PLJIT_PARSEPRINTVISITOR_H
#define PLJIT_PARSEPRINTVISITOR_H

#include <iostream>
#include <fstream>

#include "SourceCodeManager.h"
#include "ParseTreeVisitor.h"
#include "TreeNode.h"

namespace jit {

class ParsePrintVisitor : public ParseTreeVisitor {

    public:

    // Constructor          Initializes an object with a specified filename for the output and a corresponding SourceCodeManager
    explicit ParsePrintVisitor(std::string filename, SourceCodeManager& manager) :  filename{move(filename)}, manager{manager}{}

    // printTree            Prints the parse tree defined by the given node a root
    void printTree(TreeNode& root);

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
    void visit(const FuncDeclNode& node) override;


    private:

    size_t index{0};                    // Index used for numbering the nodes
    std::ofstream of{};                 // The output stream to write with
    std::string filename{};             // The name of the output file
    SourceCodeManager& manager;         // The corresponding SourceCodeManager
    std::vector<size_t> indexstack{};   // Stack that is used for a correct indexing of the nodes

    // Helper methods

    void printNonTerminalNode(std::string label);

};

} // namespace jit


#endif //PLJIT_PARSEPRINTVISITOR_H
