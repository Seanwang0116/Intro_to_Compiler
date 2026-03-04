#include "AST/VariableReference.hpp"

#include <algorithm>

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    for_each(m_indices.begin(), m_indices.end(), visit_ast_node);
}

bool VariableReferenceNode::check_index_valid(){
    for (auto &index : m_indices)
        if (index->getType() == NULL)
            return false;

    for (auto &index : m_indices)
        m_dimensions.emplace_back(static_cast<ConstantValueNode*>(index.get())->getConstantValueCString());
        
    return true;
}
