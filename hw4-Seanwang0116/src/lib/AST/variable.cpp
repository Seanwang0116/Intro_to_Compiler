#include "AST/variable.hpp"

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (m_constant_value_node_ptr) {
        m_constant_value_node_ptr->accept(p_visitor);
    }
}

void VariableNode::set_parameter(bool set){
    parameter_or_not = set;
}

void VariableNode::set_loop_var(bool set){
    loop_var_or_not = set;
}