#include "AST/variable.hpp"
#include <iostream>
// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col, std::string p_name, std::string p_type, ConstantValueNode *p_constant_value)
    : AstNode{line, col}, name(p_name), type(p_type), constant_value(p_constant_value) {}
VariableNode::VariableNode(const uint32_t line, const uint32_t col, std::string p_name)
    : AstNode{line, col}, name(p_name) {}
// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {

    std::printf("variable <line: %u, col: %u> %s %s\n",
                getLocation().line, getLocation().col,
                getName().c_str(), getType().c_str());

}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (constant_value != NULL)
        constant_value->accept(p_visitor);
}
