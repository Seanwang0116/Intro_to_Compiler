#include "AST/ConstantValue.hpp"
#include <iostream>
// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, int p_constant_value)
    : ExpressionNode{line, col}, constant_value(std::to_string(p_constant_value)) {}
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, float p_constant_value)
    : ExpressionNode{line, col}, constant_value(std::to_string(p_constant_value)) {}
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, std::string *p_constant_value)
    : ExpressionNode{line, col}, constant_value(*p_constant_value) {}

void ConstantValueNode::print() {
    std::cout << "Debug: ConstantValueNode print called" << std::endl;
    std::printf("constant <line: %u, col: %u> %s\n",
                getLocation().line,
                getLocation().col,
                getValue().c_str());
}


