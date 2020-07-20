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

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void ParsePrintVisitor::visit(const GenericTerminalNode& node) {

    // Print the label of the node
    of << index << "[label=\"" << manager.getString(node.location) << "\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";

    ++index;
}

void ParsePrintVisitor::visit(const PrimaryExprNode& node) {

    printNonTerminalNode("Primary Expression");
    /*
    // Print the label of the node
    of << index << "[label=\"Primary Expression\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();*/

}

void ParsePrintVisitor::visit(const UnaryExprNode& node) {

    printNonTerminalNode("Unary Expression");
    /*

    // Print the label of the node
    of << index << "[label=\"Unary Expression\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();*/

}

void ParsePrintVisitor::visit(const MultExprNode& node) {

    printNonTerminalNode("Multiplicative Expression");

    /*
    // Print the label of the node
    of << index << "[label=\"Multiplicative Expression\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();*/


}

void ParsePrintVisitor::visit(const AdditiveExprNode& node) {

    // Print the label of the node
    of << index << "[label=\"Additive Expression\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


}

void ParsePrintVisitor::visit(const AssignExprNode& node) {

    of << index << "[label=\"Assign Expression\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


}

void ParsePrintVisitor::visit(const Statement& node) {

    of << index << "[label=\"Statement\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


}

void ParsePrintVisitor::visit(const StatementList& node) {

    of << index << "[label=\"Statement List\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


}

void ParsePrintVisitor::visit(const CompoundStatement& node) {

    of << index << "[label=\"Compound Statement\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


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

    of << "digraph {\n\n";

    root.accept(*this);



    of << "}\n";

    of.close();

}
void ParsePrintVisitor::printNonTerminalNode(std::string label, ) {

    of << index << "[label=\"" << label << "\"]\n";

    if (!indexstack.empty())
        of << indexstack.back() << " -> " << index << "\n";


    indexstack.push_back(index);

    ++index;

    for (auto& n : node.nodes)
        n->accept(*this);

    indexstack.pop_back();


}

} // namespace jit