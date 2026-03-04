#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col, ExpressionNode *p_expression, CompoundStatementNode *p_compound_statement)
    : AstNode{line, col}, expression(p_expression), compound_statement(p_compound_statement) {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {
    std::printf("while statement <line: %u, col: %u>\n",
                getLocation().line, getLocation().col);
}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expression->accept(p_visitor);
    if (compound_statement != NULL)
        compound_statement->accept(p_visitor);
}
