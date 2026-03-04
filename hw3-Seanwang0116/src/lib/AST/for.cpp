#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col, DeclNode *p_declaration, AssignmentNode *p_assignment, ConstantValueNode *p_expression, CompoundStatementNode *p_compound_statement)
    : AstNode{line, col}, declaration(p_declaration), statement(p_assignment), expression(p_expression), compound_statement(p_compound_statement) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {

    std::printf("for statement <line: %u, col: %u>\n", getLocation().line,
                getLocation().col);

}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (declaration != nullptr) 
        declaration->accept(p_visitor);

    if (statement != nullptr)
        statement->accept(p_visitor);

    if (expression != nullptr)
        expression->accept(p_visitor);

    if (compound_statement != nullptr)
        compound_statement->accept(p_visitor);
}
