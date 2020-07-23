#ifndef PLJIT_ASTPRINTVISITOR_H
#define PLJIT_ASTPRINTVISITOR_H

#include <string>
#include <fstream>

#include "AstVisitor.h"
#include "AstNode.h"
#include "../SourceCodeManager.h"

namespace jit {

class AstPrintVisitor : public AstVisitor {

    public:

    // Constructor          Initializes an object with a specified filename for the output and a corresponding SourceCodeManager
    explicit AstPrintVisitor(std::string filename, SourceCodeManager& manager) :  filename{move(filename)}, manager{manager}{}

    // printTree            Prints the parse tree defined by the given node a root
    //void printTree(AstFunction root);

    void printExpression(AstArithmeticExpression& root);
    void printStatement(AstStatement& root);
    void printFunction(AstFunction& root);

    void visit(const AstLiteral& node) override;
    void visit(const AstIdentifier& node) override ;
    void visit(const AstUnaryArithmeticExpression& node) override ;
    void visit(const AstBinaryArithmeticExpression& node) override;
    void visit(const AstReturn& node) override ;
    void visit(const AstAssignment& node) override ;

    private:

    size_t index{0};                    // Index used for numbering the nodes
    std::ofstream of{};                 // The output stream to write with
    std::string filename{};             // The name of the output file
    SourceCodeManager& manager;         // The corresponding SourceCodeManager
    std::vector<size_t> indexstack{};   // Stack that is used for a correct indexing of the nodes


};

} // namespace jit
#endif //PLJIT_ASTPRINTVISITOR_H
