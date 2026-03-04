#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>
class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          /* TODO: name */
                          char *const p_name);
    // array reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          /* TODO: name, expressions */
                          char *const p_name, std::vector<ExpressionNode* > *p_expression);
    ~VariableReferenceNode() = default;

    void print() override;
    std::string getName() { return variable_name; }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: variable name, expressions
    std::string variable_name;
    std::vector<ExpressionNode* > *expressions;
};

#endif
