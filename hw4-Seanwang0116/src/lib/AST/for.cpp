#include "AST/for.hpp"

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    m_loop_var_decl->set_loop_var(true);
    m_loop_var_decl->accept(p_visitor);
    if (!m_loop_var_decl->faulty) {
        m_init_stmt->accept(p_visitor);
        m_end_condition->accept(p_visitor);
    }
    m_body->accept(p_visitor);
}

bool ForNode::check_bound() {
    ConstantValueNode* start = dynamic_cast<ConstantValueNode*>(m_init_stmt->getExpr());
    int start_value = atoi(start->getConstantValueCString());

    ConstantValueNode* end = dynamic_cast<ConstantValueNode*>(m_end_condition.get());
    int end_value = atoi(end->getConstantValueCString());

    if (start_value > end_value) {
        return false;
    }
    return true;
}