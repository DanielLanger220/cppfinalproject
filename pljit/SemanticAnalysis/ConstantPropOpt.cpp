#include "ConstantPropOpt.h"

using namespace std;

namespace jit {


void ConstantPropOpt::visit(AstLiteral& node) {

    exprmap.insert(pair<AstNode*, int64_t>(&node, node.value));

}

void ConstantPropOpt::visit(AstIdentifier& node) {

    // Check, if the identifier is a constant or a variable currently marked as constant
    if (node.index >= conststart || (node.index >= varstart && vartable[node.index]))
        exprmap.insert(pair<AstNode*, int64_t>(&node, *vartable[node.index]));

}

void ConstantPropOpt::visit(AstUnaryArithmeticExpression& node) {

    if(firstRun) {

        node.subexpr->optimise(*this);

        auto it = exprmap.find(node.subexpr.get());

        // Check if the subexpression is marked as constant. If so, mark this unary expression as constant as well
        if (it != exprmap.end())
            exprmap.insert(pair<AstNode*, int64_t>(&node, - it->second.value()));
    }
    else {

        // If this unary expression (referred to by the node reference) cannot be made constant, it is impossible that the subexpression can be made constant.
        // So just descend into the subexpression and check if subexpressions of the subexpression can be optimised
        node.subexpr->optimise(*this);

    }

}

void ConstantPropOpt::visit(AstBinaryArithmeticExpression& node) {

    if (firstRun) {

        node.lhs->optimise(*this);
        node.rhs->optimise(*this);

        auto itleft = exprmap.find(node.lhs.get());
        auto itright = exprmap.find(node.rhs.get());

        bool divbyzero{false};

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
    else {

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

    if (firstRun) {

        // Optimise the return value of the return statement
        node.returnvalue->optimise(*this);
    }
    else {

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

    if (firstRun) {

        // optimise the expression on the right hand side
        node.rhs->optimise(*this);

        // Check if the right hand side expression is a constant value
        auto it = exprmap.find(node.rhs.get());

        // if the expression is a constant value, mark the identifier on the left hand side as constant
        if (it != exprmap.end())
            vartable[node.lhs->index] = it->second.value();
        else    // Otherwise mark the identifier on the left hand side as non-constant
            vartable[node.lhs->index] = nullopt;

    }
    else {


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

    vartable = vector<optional<int64_t>>(node.nofidentifier, nullopt);
    exprmap.clear();

    varstart = node.nofparameters;
    conststart = node.nofparameters + node.nofvariables;

    // Initialise the table with the constant values
    for(size_t i = 0; i < node.constants->size(); ++i)
        vartable[node.nofparameters + node.nofvariables + i] = (*node.constants)[i];


    // Do the first run over the nodes
    firstRun = true;
    node.statementlist->optimise(*this);

    // Do the second run over the nodes
    firstRun = false;
    node.statementlist->optimise(*this);

}




} // namespace jit