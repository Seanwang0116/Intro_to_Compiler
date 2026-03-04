#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col, VariableReferenceNode *p_variable_reference, ExpressionNode *p_expression)
    : AstNode{line, col}, variable_reference(p_variable_reference), expression(p_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {
    std::printf("assignment statement <line: %u, col: %u>\n",
                getLocation().line,
                getLocation().col);
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (variable_reference != NULL) {
       variable_reference->accept(p_visitor);
    }
    if (expression != NULL) {
        expression->accept(p_visitor);
    }
}
