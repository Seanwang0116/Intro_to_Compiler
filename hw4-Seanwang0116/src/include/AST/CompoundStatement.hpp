#ifndef AST_COMPOUND_STATEMENT_NODE_H
#define AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"

#include <vector>
#include <memory>

class CompoundStatementNode final : public AstNode {
  public:
    using DeclNodes = std::vector<std::unique_ptr<DeclNode>>;
    using StmtNodes = std::vector<std::unique_ptr<AstNode>>;

  private:
    DeclNodes m_decl_nodes;
    StmtNodes m_stmt_nodes;
    bool function_or_not = false;

  public:
    ~CompoundStatementNode() = default;
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          DeclNodes &p_decl_nodes, StmtNodes &p_stmt_nodes)
        : AstNode{line, col}, m_decl_nodes(std::move(p_decl_nodes)),
          m_stmt_nodes(std::move(p_stmt_nodes)){}

    void set_function(bool set);
    bool check_function_or_not() { return function_or_not; }

    void accept(AstNodeVisitor &p_visitor) override {
        p_visitor.visit(*this);
    }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
