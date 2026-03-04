#include <AST/expression.hpp>

void ExpressionNode::set_type(PTypeSharedPtr type) {
    m_type = type;
}

bool ExpressionNode::scalar_or_not() {
    return m_type->scalar_or_not();
}