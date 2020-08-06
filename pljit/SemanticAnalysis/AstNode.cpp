#include "AstNode.h"
#include "../Evaluation/EvalInstance.h"

using namespace std;

namespace jit {

std::optional<int64_t> AstLiteral::evaluate(EvalInstance&) {

    return value;
}

optional<int64_t> AstIdentifier::evaluate(EvalInstance& instance) {

    return instance.getValue(index);
}

optional<int64_t> AstBinaryArithmeticExpression::evaluate(EvalInstance& instance) {

    auto leftvalue = lhs->evaluate(instance);
    auto rightvalue = rhs->evaluate(instance);

    if (!leftvalue || !rightvalue)
        return nullopt;

    switch(op) {

        case ArithmeticOperation::Plus:
            return leftvalue.value() + rightvalue.value();
        case ArithmeticOperation::Minus:
            return leftvalue.value() - rightvalue.value();
        case ArithmeticOperation::Mul:
            return leftvalue.value() * rightvalue.value();
        case ArithmeticOperation::Div:
            if (rightvalue == 0) {
                instance.manager.printErrorMessage("error: Division by 0", rhs->location);
                return nullopt;
            }
            else
                return leftvalue.value() / rightvalue.value();
        default:
            return nullopt;
    }

}
optional<int64_t> AstUnaryArithmeticExpression::evaluate(EvalInstance& instance) {

    auto subvalue = subexpr->evaluate(instance);

    if (!subvalue)
        return nullopt;

    return -subvalue.value();
}

optional<int64_t> AstAssignment::evaluate(EvalInstance& instance) {

    auto value = rhs->evaluate(instance);

    if (!value)
        return nullopt;

    instance.setValue(static_cast<AstIdentifier&>(*lhs).index, value.value());

    return value;

}
optional<int64_t> AstReturn::evaluate(EvalInstance& instance) {

    auto result = returnvalue->evaluate(instance);

    return result;
}

optional<int64_t> AstStatementList::evaluate(EvalInstance& instance) {

    for (auto& st : statements) {

        auto result = st->evaluate(instance);

        if (st->subtype == AstStatement::SubType::AstReturn)
            return result;
    }

    return nullopt;
}

optional<int64_t> AstFunction::evaluate(EvalInstance& instance) {

    return statementlist->evaluate(instance);
}


} // namespace jit