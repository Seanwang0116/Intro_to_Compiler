#include "AST/BinaryOperator.hpp"

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(p_visitor); };

    visit_ast_node(m_left_operand);
    visit_ast_node(m_right_operand);
}

bool BinaryOperatorNode::check_child() {
    if(m_left_operand->getType() == NULL || m_right_operand->getType() == NULL)
        return false;
    else
        return true;
}

bool BinaryOperatorNode::check_type() {
    switch(m_op) {
        case Operator::kPlusOp:
            if (m_left_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kStringType && m_right_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kStringType) {
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kStringType));
                return true;
            }
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            if (m_left_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType || m_right_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType)
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kRealType));
            else
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
        break;
        case Operator::kMinusOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            if (m_left_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType || m_right_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType)
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kRealType));
            else
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
        break;
        case Operator::kMultiplyOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            if (m_left_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType || m_right_operand->getType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType)
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kRealType));
            else
                this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
        break;
        case Operator::kDivideOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
        break;
        case Operator::kModOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kIntegerType));
        break;
        case Operator::kAndOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kOrOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kEqualOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kGreaterOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kGreaterOrEqualOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kLessOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kLessOrEqualOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        case Operator::kNotEqualOp:
            if (m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_left_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kRealType && m_right_operand->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kIntegerType)
                return false;
            else if (m_left_operand->getType()->getDimensions().size() != 0 || m_right_operand->getType()->getDimensions().size() != 0)
                return false;
            this->set_type(std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
        break;
        default:
            return false;
    }
    return true;
}