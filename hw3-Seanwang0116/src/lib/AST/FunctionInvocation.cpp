#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col, char *const p_name, std::vector<ExpressionNode *> *p_expression)
    : ExpressionNode{line, col}, function_name(p_name), expressions(p_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {
    std::printf("function invocation <line: %u, col: %u> %s\n",
                getLocation().line,
                getLocation().col,
                getNameCString());
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (expressions != NULL){
        for (auto &expression : *expressions) {
            expression->accept(p_visitor);
        }
    }
}
