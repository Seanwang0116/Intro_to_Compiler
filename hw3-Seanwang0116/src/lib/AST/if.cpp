#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, ExpressionNode *p_expression, CompoundStatementNode *p_compound_statement, CompoundStatementNode *p_compound_statement_else)
    : AstNode{line, col}, expression(p_expression), compound_statement_body(p_compound_statement), compound_statement_else(p_compound_statement_else) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {
    std::printf("if statement <line: %u, col: %u>\n", getLocation().line,
                getLocation().col);
}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expression != nullptr) {
        expression->accept(p_visitor);
    }
    if (compound_statement_body != nullptr) {
        compound_statement_body->accept(p_visitor);
    }
    if (compound_statement_else != nullptr) {
        compound_statement_else->accept(p_visitor);
    }
}
