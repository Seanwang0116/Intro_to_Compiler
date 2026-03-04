#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col, std::vector<DeclNode*>* p_declarations, std::vector<AstNode*>* p_statements)
    : AstNode{line, col}, declarations(p_declarations), statements(p_statements) {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {
     std::printf("compound statement <line: %u, col: %u>\n",
                getLocation().line,
                getLocation().col);
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
     if (declarations != NULL) {
        for (auto &decl : *declarations) {
            decl->accept(p_visitor);
        }
    }
    if (statements != NULL) {
        for (auto &stmt : *statements) {
            stmt->accept(p_visitor);
        }
    }
}
