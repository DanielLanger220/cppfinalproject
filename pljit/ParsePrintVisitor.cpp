#include "ParsePrintVisitor.h"

namespace jit {

void ParsePrintVisitor::visit(const IdentifierNode& node)  {

    // Print the label of the node
    of << index << "[label=\"" << node.id << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void ParsePrintVisitor::visit(const LiteralNode& node) {

    // Print the label of the node
    of << index << "[label=\"" << node.value << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void ParsePrintVisitor::visit(const GenericTerminalNode& node) {

    // Print the label of the node
    of << index << "[label=\"" << manager.getString(node.location) << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void ParsePrintVisitor::visit(const PrimaryExprNode& node) {

    printNonTerminalNode("Primary Expression", node);
}

void ParsePrintVisitor::visit(const UnaryExprNode& node) {

    printNonTerminalNode("Unary Expression", node);
}

void ParsePrintVisitor::visit(const MultExprNode& node) {

    printNonTerminalNode("Multiplicative Expression", node);
}

void ParsePrintVisitor::visit(const AdditiveExprNode& node) {

    printNonTerminalNode("Additive Expression", node);
}

void ParsePrintVisitor::visit(const AssignExprNode& node) {

    printNonTerminalNode("Assignment Expression", node);
}

void ParsePrintVisitor::visit(const Statement& node) {

    printNonTerminalNode("Statement", node);
}

void ParsePrintVisitor::visit(const StatementList& node) {

    printNonTerminalNode("Statement-List", node);
}

void ParsePrintVisitor::visit(const CompoundStatement& node) {

    printNonTerminalNode("Compound Statement", node);
}

void ParsePrintVisitor::visit(const ParamDeclNode& node) {


}

void ParsePrintVisitor::visit(const VarDeclNode& node) {

}

void ParsePrintVisitor::visit(const ConstDeclNode& node) {

}

void ParsePrintVisitor::visit(const DeclListNode& node) {

}

void ParsePrintVisitor::visit(const FuncDeclNode& node) {

}
void ParsePrintVisitor::printTree(TreeNode& root) {

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
void ParsePrintVisitor::printNonTerminalNode(std::string label, const NonTerminalTreeNode& node ) {

    // Print the label of the node
    of << index << "[label=\"" << label << "\"]\n";

    // If the node has a parent node, print the edge from the parent node to this node
    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    // Push the index of this node to the stack (as parent node for the direct child nodes)
    indexstack.push_back(index++);

    for (auto& n : node.nodes)
        n->accept(*this);

    // Remove the index of the current node from the stack
    indexstack.pop_back();
}

} // namespace jit