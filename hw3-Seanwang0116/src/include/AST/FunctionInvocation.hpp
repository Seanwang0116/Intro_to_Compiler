#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>
class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                          /* TODO: function name, expressions */
                          char *const p_name, std::vector<ExpressionNode* > *p_expression);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() { return function_name.c_str(); };

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: function name, expressions
    std::string function_name;
    std::vector<ExpressionNode* > *expressions;
};

#endif
