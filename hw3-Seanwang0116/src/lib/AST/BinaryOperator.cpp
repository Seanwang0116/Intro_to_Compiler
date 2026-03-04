#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col, std::string p_operator, ExpressionNode *p_left, ExpressionNode *p_right)
    : ExpressionNode{line, col}, operator_bi(p_operator), left(p_left), right(p_right) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {

    std::printf("binary operator <line: %u, col: %u> %s\n",
                getLocation().line, getLocation().col,
                getOperator().c_str());
                
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (left != NULL)
        left->accept(p_visitor);
    if (right != NULL)
        right->accept(p_visitor);
}
