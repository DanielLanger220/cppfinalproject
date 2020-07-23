#include "AstPrintVisitor.h"

using namespace std;

namespace jit{

void AstPrintVisitor::visit(const AstLiteral& node) {

    // Print the label of the node
    of << index << " [label=\"" << manager.getString(node.location) << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void AstPrintVisitor::visit(const AstIdentifier& node) {

    // Print the label of the node
    of << index << " [label=\"" << manager.getString(node.location) << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;

}


void AstPrintVisitor::visit(const AstUnaryArithmeticExpression& node) {

    // Print the label of the node
    of << index << " [label=\"-\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    // Push the index of this node to the stack (as parent node for the direct child nodes)
    indexstack.push_back(index++);

    node.subexpr->accept(*this);

    // Remove the index of the current node from the stack
    indexstack.pop_back();

}


void AstPrintVisitor::visit(const AstBinaryArithmeticExpression& node) {

    // Print the label of the node

    char op;

    switch(node.op) {

        case AstBinaryArithmeticExpression::ArithmeticOperation::Plus:
            op = '+';
            break;

        case AstBinaryArithmeticExpression::ArithmeticOperation::Minus:
            op = '-';
            break;

        case AstBinaryArithmeticExpression::ArithmeticOperation::Mul:
            op = '*';
            break;

        case AstBinaryArithmeticExpression::ArithmeticOperation::Div:
            op = '/';
            break;
    }

    of << index << " [label=\"" << op << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    // Push the index of this node to the stack (as parent node for the direct child nodes)
    indexstack.push_back(index++);

    node.lhs->accept(*this);
    node.rhs->accept(*this);

    // Remove the index of the current node from the stack
    indexstack.pop_back();


}


void AstPrintVisitor::visit(const AstReturn& node) {

    // Print the label of the node
    of << index << " [label=\"Return\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    // Push the index of this node to the stack (as parent node for the direct child nodes)
    indexstack.push_back(index++);

    node.returnvalue->accept(*this);

    // Remove the index of the current node from the stack
    indexstack.pop_back();

}


void AstPrintVisitor::visit(const AstAssignment& node) {

    // Print the label of the node
    of << index << " [label=\":=\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    // Push the index of this node to the stack (as parent node for the direct child nodes)
    indexstack.push_back(index++);

    node.lhs->accept(*this);
    node.rhs->accept(*this);

    // Remove the index of the current node from the stack
    indexstack.pop_back();

}

void AstPrintVisitor::printExpression(AstArithmeticExpression& root) {

    of.open(filename);
    index = 0;

    // Print header of .dot file
    of << "digraph {\n\n";

    // Print the nodes and edges of the parse tree with the given node as root
    root.accept(*this);

    // Print tail of .dot file
    of << "}\n";

    of.close();
}

void AstPrintVisitor::printStatement(AstStatement& root) {

    of.open(filename);
    index = 0;

    // Print header of .dot file
    of << "digraph {\n\n";

    // Print the nodes and edges of the parse tree with the given node as root
    root.accept(*this);

    // Print tail of .dot file
    of << "}\n";

    of.close();


}

void AstPrintVisitor::printFunction(AstFunction& root) {

    of.open(filename);
    index = 1;
    indexstack.clear();
    indexstack.push_back(0);

    // Print header of .dot file
    of << "digraph {\n\n";
    of << "0 [label = \"Function\"]\n";

    // Print the nodes and edges of the parse tree with the given node as root
    for (auto &s : root.statements)
        s->accept(*this);


    // Print tail of .dot file
    of << "}\n";

    of.close();


}


} // namespace jit