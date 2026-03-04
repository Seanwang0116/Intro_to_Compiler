#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col, const char* name, std::vector<DeclNode*>* declaration_func, Ptype *p_return_type, CompoundStatementNode* p_compound_statement)
    : AstNode{line, col} {
        this->name = name;
        this->declaration_func = declaration_func;
        this->compound_statement = p_compound_statement;

        switch (*p_return_type) {
            case INT:
                this->return_type = "integer";
                break;
            case REAL:
                this->return_type = "real";
                break;
            case STRING:
                this->return_type = "string";
                break;
            case BOOLEAN:
                this->return_type = "boolean";
                break;
            case VOID:
                this->return_type = "void";
                break;
            default:
                this->return_type = "unknown";
                break;
        }
    }

std::string FunctionNode::getProto() {
    std::string proto = return_type + " (";
    if (declaration_func != nullptr && !declaration_func->empty()) {
        for (const auto& declaration : *declaration_func)
            proto += declaration->getProto() + ", ";
        proto.erase(proto.size() - 2);
    }
    proto += ")";
    return proto;
}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (declaration_func != nullptr)
        for (auto &dec : *declaration_func)
            dec->accept(p_visitor);
            
    if (compound_statement != nullptr)
        compound_statement->accept(p_visitor);
}
