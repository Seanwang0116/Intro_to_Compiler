#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col, std::string p_operator, ExpressionNode *p_expression)
    : ExpressionNode{line, col}, operator_u(p_operator), expression(p_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {

    std::printf("unary operator <line: %u, col: %u> %s\n",
                getLocation().line, getLocation().col,
                getOperator().c_str());
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expression != NULL)
        expression->accept(p_visitor);
}
