#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/decl.hpp"

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */
                 const char* name, std::vector<DeclNode*>* declaration_func, Ptype *p_type, CompoundStatementNode* p_compound_statement);
    ~FunctionNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    std::string getProto();
    std::string getName() { return name; }

  private:
    // TODO: name, declarations, return type, compound statement
    std::string name;
    std::vector<DeclNode*>* declaration_func;
    std::string return_type;
    CompoundStatementNode* compound_statement;
};

#endif
