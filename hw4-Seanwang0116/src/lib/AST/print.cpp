#include "AST/print.hpp"

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    m_target->accept(p_visitor);
}

bool PrintNode::check_expression() {
    if(m_target->getType() == NULL)
        return false;
    else
        return true;
}

bool PrintNode::check_scalar() {
    if (m_target->getType()->scalar_or_not()) {
        return true;
    }
    return false;
}