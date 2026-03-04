#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/adder.hpp"
#include <iostream>

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
            /* TODO: identifiers, type */
            std::vector<VariableNode *> *p_variable_name, Ptype *p_type, std::vector<int> *size_of_array);
    // constant variable declaration
    DeclNode(const uint32_t, const uint32_t col,
             /* TODO: identifiers, constant */
             std::vector<VariableNode *> *p_variable_name, Ptype *p_type, ConstantValueNode *p_constant_value);
    DeclNode(const uint32_t, const uint32_t col,
            std::vector<VariableNode *> *p_variable_name, Ptype *p_type);

    ~DeclNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    std::string getProto();
    void print() override;

  private:
    // TODO: variables
    std::vector<VariableNode* > *variables;
};

#endif
