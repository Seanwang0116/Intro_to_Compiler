#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col, char *p_name)
    : ExpressionNode{line, col}, variable_name(p_name), expressions(NULL)  {}

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col, char *p_name, std::vector<ExpressionNode *> *p_expression)
    : ExpressionNode{line, col}, variable_name(p_name), expressions(p_expression)  {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {
    std::printf("variable reference <line: %u, col: %u> %s\n",
                getLocation().line,
                getLocation().col,
                getName().c_str());
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expressions != NULL) {
        for (auto &expression : *expressions) {
            expression->accept(p_visitor);
        }
    }
}
