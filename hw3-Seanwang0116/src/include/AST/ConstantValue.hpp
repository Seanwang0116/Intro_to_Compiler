#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      /* TODO: constant value */
                      int p_constant_value);
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      float p_constant_value);
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      std::string *p_constant_value);
    ~ConstantValueNode() = default;

    void print() override;
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override {};
    std::string getValue() const { return constant_value; }
  private:
    // TODO: constant value
    std::string constant_value;
};

#endif
