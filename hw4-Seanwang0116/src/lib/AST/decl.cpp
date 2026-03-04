#include "AST/decl.hpp"

#include <algorithm>

void DeclNode::init(const std::vector<IdInfo> *const p_ids,
                    const PTypeSharedPtr &p_type,
                    ConstantValueNode *const p_constant) {
    std::shared_ptr<ConstantValueNode> shared_constant(p_constant);

    auto make_variable_node_and_emplace_back_in_var_nodes =
        [&](const IdInfo &id_info) {
            m_var_nodes.emplace_back(
                new VariableNode(id_info.location.line, id_info.location.col,
                                 id_info.id, p_type, shared_constant));
        };

    for_each(p_ids->begin(), p_ids->end(),
             make_variable_node_and_emplace_back_in_var_nodes);
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_var_node = [&](auto &var_node) { var_node->accept(p_visitor); };
    for_each(m_var_nodes.begin(), m_var_nodes.end(), visit_var_node);
    for (auto &node : m_var_nodes)
        if (node->faulty)
            faulty = true;
}

void DeclNode::set_parameter(bool set){
    for(auto& node : m_var_nodes)
        node->set_parameter(set);
}

void DeclNode::set_loop_var(bool set){
    for(auto& node : m_var_nodes)
        node->set_loop_var(set);
}