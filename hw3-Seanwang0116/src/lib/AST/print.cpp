#include "AST/print.hpp"
#include <cstdio>
// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, ExpressionNode *p_expression)
    : AstNode{line, col}, expression(p_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {

    printf("print statement <line: %u, col: %u>\n",
                getLocation().line, getLocation().col);

}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expression != nullptr){
        expression->accept(p_visitor);
    }
}
