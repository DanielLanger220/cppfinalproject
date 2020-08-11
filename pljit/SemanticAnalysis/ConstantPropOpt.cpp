#include "ConstantPropOpt.h"

using namespace std;

namespace jit {


void ConstantPropOpt::visit(AstLiteral& node) {

    // A literal node is always constant
    exprmap.insert(pair<AstNode*, int64_t>(&node, node.value));
}

void ConstantPropOpt::visit(AstIdentifier& node) {

    // Check, if the identifier is currently marked as constant. If so, mark the expression as constant
    if (vartable[node.index])
        exprmap.insert(pair<AstNode*, int64_t>(&node, *vartable[node.index]));

}

void ConstantPropOpt::visit(AstUnaryArithmeticExpression& node) {

    if(firstRun) {  // First run

        // Optimise the subexpression
        node.subexpr->optimise(*this);

        // Check if the subexpression is marked as constant. If so, mark this unary expression as constant as well
        auto it = exprmap.find(node.subexpr.get());

        if (it != exprmap.end())
            exprmap.insert(pair<AstNode*, int64_t>(&node, - it->second.value()));
    }
    else {  // Second run

        // If this unary expression (referred to by the node reference) cannot be made constant, it is impossible that the subexpression can be made constant.
        // So just descend into the subexpression and check if subexpressions of the subexpression can be optimised
        node.subexpr->optimise(*this);

    }

}

void ConstantPropOpt::visit(AstBinaryArithmeticExpression& node) {

    if (firstRun) { // First run

        // Optimise the subexpressions
        node.lhs->optimise(*this);
        node.rhs->optimise(*this);

        auto itleft = exprmap.find(node.lhs.get());
        auto itright = exprmap.find(node.rhs.get());

        bool divbyzero{false};

        // If both subexpression are constant then the expression is also constant
        if (itleft != exprmap.end() && itright != exprmap.end()) {

            int64_t leftres = itleft->second.value();
            int64_t rightres = itright->second.value();
            int64_t result{};

            switch(node.op) {

                case AstBinaryArithmeticExpression::ArithmeticOperation::Plus:
                    result = leftres + rightres;
                    break;
                case AstBinaryArithmeticExpression::ArithmeticOperation::Minus:
                    result = leftres - rightres;
                    break;
                case AstBinaryArithmeticExpression::ArithmeticOperation::Mul:
                    result = leftres * rightres;
                    break;
                case AstBinaryArithmeticExpression::ArithmeticOperation::Div:
                    if (rightres == 0)
                        divbyzero = true;
                    else
                        result = leftres / rightres;
                    break;
            }

            if (!divbyzero)
                exprmap.insert(pair<AstNode*, optional<int64_t>>(&node, result));
        }
    }
    else {  // Second run

        // Check if the left subexpression can be made constant, otherwise descend into it
        auto it = exprmap.find(node.lhs.get());

        if (it != exprmap.end()) {

            SourceCodeReference ref = node.lhs->location;
            node.lhs = make_unique<AstLiteral>(ref, it->second.value());
        }
        else
            node.lhs->optimise(*this);


        // Check if the right subexpression can be made constant, otherwise descend into it
        it = exprmap.find(node.rhs.get());

        if (it != exprmap.end()) {

            SourceCodeReference ref = node.rhs->location;
            node.rhs = make_unique<AstLiteral>(ref, it->second.value());
        }
        else
            node.rhs->optimise(*this);
    }

}

void ConstantPropOpt::visit(AstReturn& node) {

    if (firstRun) { // First run

        // Optimise the return value of the return statement
        node.returnvalue->optimise(*this);
    }
    else {  // Second run

        // Check if the return value expression can be merged into a literal node, otherwise descend into it
        auto it = exprmap.find(node.returnvalue.get());

        if (it != exprmap.end()) {

            SourceCodeReference location = node.returnvalue->location;
            node.returnvalue = make_unique<AstLiteral>(location, it->second.value());
        }
        else
            node.returnvalue->optimise(*this);
    }

}

void ConstantPropOpt::visit(AstAssignment& node) {

    if (firstRun) { // First run

        // optimise the expression on the right hand side
        node.rhs->optimise(*this);

        // Check if the right hand side expression is a constant value
        auto it = exprmap.find(node.rhs.get());

        // if the expression is a constant value, mark the identifier on the left hand side as constant
        if (it != exprmap.end())
            vartable[static_cast<AstIdentifier&>(*node.lhs).index] = it->second.value();
        else    // Otherwise mark the identifier on the left hand side as non-constant
            vartable[static_cast<AstIdentifier&>(*node.lhs).index] = nullopt;

    }
    else { // Second run

        // Check if the expression on the right hand side can be merged into a literal node, otherwise descend into it
        auto it = exprmap.find(node.rhs.get());

        if (it != exprmap.end()) {

            SourceCodeReference location = node.rhs->location;
            node.rhs = make_unique<AstLiteral>(location, it->second.value());
        }
        else
            node.rhs->optimise(*this);
    }

}

void ConstantPropOpt::visit(AstStatementList& node) {

    // Iterate over all statements in order
    for(auto& st : node.statements)
        st->optimise(*this);
}


void ConstantPropOpt::visit(AstFunction& node) {

    vartable = vector<optional<int64_t>>(node.nofidentifiers, nullopt);
    exprmap.clear();

    // Do the first run over the nodes
    firstRun = true;
    node.statementlist->optimise(*this);

    // Do the second run over the nodes
    firstRun = false;
    node.statementlist->optimise(*this);

}

} // namespace jit