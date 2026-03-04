#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, VariableReferenceNode *p_variable_reference)
    : AstNode{line, col}, variable_reference(p_variable_reference) {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {

    std::printf("read statement <line: %u, col: %u>\n",
                getLocation().line, getLocation().col);

}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (variable_reference != NULL) {
        variable_reference->accept(p_visitor);
    }
}
