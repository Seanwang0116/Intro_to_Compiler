#include "AST/decl.hpp"
#include <iostream>
// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode *> *p_variable_name, Ptype *p_type, std::vector<int> *size_of_array)
    : AstNode{line, col} 
    {
        std::string type;
        switch (*p_type) {
            case INT:    type = "integer"; break;
            case REAL:   type = "real"; break;
            case STRING: type = "string"; break;
            case BOOLEAN: type = "boolean"; break;
            default:      type = "unknown"; break;
        }
        if (size_of_array != NULL) {
            type += " ";
            for (auto &size : *size_of_array) {
                type += "[" + std::to_string(size) + "]";
            }
        }
        variables = p_variable_name;
        for (auto &variable : *variables) {
            variable->set_type(type);
            variable->set_constant_value(NULL);
        }
    }

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode *> *p_variable_name, Ptype *p_type, ConstantValueNode *p_constant_value)
    : AstNode{line, col}
    {
        std::string type;
        switch (*p_type) {
            case INT:    type = "integer"; break;
            case REAL:   type = "real"; break;
            case STRING: type = "string"; break;
            case BOOLEAN: type = "boolean"; break;
            default:      type = "unknown"; break;
        }
        variables = p_variable_name;
        for (auto &variable : *variables) {
            variable->set_type(type);
            variable->set_constant_value(p_constant_value);
        }
    }

DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode *> *p_variable_name, Ptype *p_type)
    : AstNode{line, col}
    {
        std::string type;
        switch (*p_type) {
            case INT:    type = "integer"; break;
            case REAL:   type = "real"; break;
            case STRING: type = "string"; break;
            case BOOLEAN: type = "boolean"; break;
            default:      type = "unknown"; break;
        }
        variables = p_variable_name;
        for (auto &variable : *variables) {
            variable->set_type(type);
            variable->set_constant_value(NULL);
        }
    }

std::string DeclNode::getProto(){
      std::string proto;
      if (variables != nullptr && !variables->empty()) {
        for (const auto& variable : *variables)
          proto += variable->getType() + ", ";
        proto.erase(proto.size() - 2);
      }
      return proto;
    }
// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {

    std::printf("declaration <line: %u, col: %u>\n", getLocation().line,
                getLocation().col);

}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (variables != NULL)
        for (auto &variable : *variables)
            variable->accept(p_visitor);
 }
