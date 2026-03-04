#include "AST/UnaryOperator.hpp"

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    visit_ast_node(m_operand);
}

bool UnaryOperatorNode::check_child() {
    if(m_operand->getType() == NULL)
        return false;
    else
        return true;
}

bool UnaryOperatorNode::check_type() {
    switch(m_op) {
        case Operator::kNotOp:
            if (m_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                return false;
            else if (m_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kNegOp:
            if (m_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType && m_operand->getType()->getDimensions().size() == 0){
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kRealType));
                return true;
            }
            else if (m_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType && m_operand->getType()->getDimensions().size() == 0){
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
                return true;
            }
            else
                return false;
        break;
        default:
            return false;
    }
    return true;
}