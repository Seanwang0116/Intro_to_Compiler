#include "sema/Error.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    symbol_manager.pushScope(new SymbolTable(p_program.getReturnType()));
    symbol_manager.insertSymbol(p_program.getNameCString(), "program", symbol_manager.getLevel(), p_program.getType(), "");

    p_program.visitChildNodes(*this);

    symbol_manager.printCurrentScope();
    symbol_manager.popScope();
    if(faulty)
        exit(1);

}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    for(auto& node : loop_vars){
        if(node == p_variable.getNameCString()){
            m_error_printer.print(SymbolRedeclarationError(p_variable.getLocation(), p_variable.getNameCString()));
            p_variable.faulty = true;
            faulty = true;
            return;
        }
    }
    if (symbol_manager.lookupSymbol(p_variable.getNameCString())) {
        m_error_printer.print(SymbolRedeclarationError(p_variable.getLocation(), p_variable.getNameCString()));
        p_variable.faulty = true;
        faulty = true;
    } else {
        symbol_manager.insertSymbol(p_variable.getNameCString(), p_variable.getKind(), symbol_manager.getLevel(), p_variable.getType(), p_variable.getAttribute());
    }

    std::vector<uint64_t> dim = p_variable.getType()->getDimensions();
    for (auto i : dim) {
        if(i <= 0) {
            faulty = true;
            m_error_printer.print(NonPositiveArrayDimensionError(p_variable.getLocation(), p_variable.getNameCString()));
            break;
        }
    }
    if (strcmp(p_variable.getKind(), "loop_var") == 0) {
        loop_vars.emplace_back(p_variable.getNameCString());
        loop_var_level++;
    }

    p_variable.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_constant_value.set_type(p_constant_value.getTypeSharedPtr());
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (symbol_manager.lookupSymbol(p_function.getNameCString())) {
        m_error_printer.print(SymbolRedeclarationError(p_function.getLocation(), p_function.getNameCString()));
        faulty = true;
    }
    else {
    symbol_manager.insertSymbol(p_function.getNameCString(), "function", symbol_manager.getLevel(), p_function.getType(), p_function.getAttribute());
    }
    symbol_manager.pushScope(new SymbolTable(p_function.getReturnType()));
    p_function.visitChildNodes(*this);
    symbol_manager.printCurrentScope();
    symbol_manager.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (!p_compound_statement.check_function_or_not())
        symbol_manager.pushScope(new SymbolTable(symbol_manager.getTopType()));

    p_compound_statement.visitChildNodes(*this);

    if (!p_compound_statement.check_function_or_not()) {
        symbol_manager.printCurrentScope();
        symbol_manager.popScope();
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);

    if (p_print.check_expression() && !p_print.check_scalar()) {
        faulty = true;
        m_error_printer.print(PrintOutNonScalarTypeError(p_print.getChildLocation()));
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_bin_op.visitChildNodes(*this);

    if (p_bin_op.check_child() && !p_bin_op.check_type()) {
        faulty = true;
        m_error_printer.print(InvalidBinaryOperandError(p_bin_op.getLocation(), p_bin_op.getOperater(), p_bin_op.getLeftOperand()->getType().get(), p_bin_op.getRightOperand()->getType().get()));
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_un_op.visitChildNodes(*this);

    if(p_un_op.check_child() && !p_un_op.check_type()){
        faulty = true;
        m_error_printer.print(InvalidUnaryOperandError(p_un_op.getLocation(), p_un_op.getOperater(), p_un_op.getOperand()->getType().get()));
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_func_invocation.visitChildNodes(*this);

    SymbolEntry* entry = symbol_manager.lookup_globalSymbol(p_func_invocation.getNameCString());
    if (entry == NULL) {
        m_error_printer.print(UndeclaredSymbolError(p_func_invocation.getLocation(), p_func_invocation.getNameCString()));
        faulty = true;
        return;
    }
    if (strcmp(entry->kind, "function") != 0) {
        m_error_printer.print(NonFunctionSymbolError(p_func_invocation.getLocation(), p_func_invocation.getNameCString()));
        faulty = true;
        return;
    }

    std::vector<std::string> parameters;
    std::istringstream stream(entry->attribute);
    std::string temp;
    while (std::getline(stream, temp, ',')) {
        if (temp[0] == ' ')
            temp.erase(0, 1);
        parameters.emplace_back(temp);
    }

    if(p_func_invocation.check_argument_num(parameters.size())){
        std::vector<ExpressionNode*> args = p_func_invocation.getArguments();
        for (int i = 0; i < static_cast<int>(args.size()); ++i) {
            auto arg_type = args[i]->getType();
            if (!arg_type) 
                return;

            const char* arg_type_cstr = arg_type->getPTypeCString();
            const std::string& expected_type = parameters[i];

            if (strcmp(arg_type_cstr, expected_type.c_str()) != 0) {
                if (strcmp(arg_type_cstr, "integer") == 0 && expected_type == "real")
                    continue;

                faulty = true;
                m_error_printer.print(IncompatibleArgumentTypeError(args[i]->getLocation(), arg_type.get(), expected_type));
                return;
            }
        }
    } else {
        faulty = true;
        m_error_printer.print(ArgumentNumberMismatchError(p_func_invocation.getLocation(), p_func_invocation.getNameCString()));
        return;
    }

    PType reference = *entry->type;
    std::shared_ptr<PType> type = std::make_shared<PType>(reference);
    p_func_invocation.set_type(type);
    return;
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_variable_ref.visitChildNodes(*this);

    SymbolEntry* entry = symbol_manager.lookup_globalSymbol(p_variable_ref.getNameCString());
    if (entry == NULL) {
        faulty = true;
        m_error_printer.print(UndeclaredSymbolError(p_variable_ref.getLocation(), p_variable_ref.getNameCString()));
        return;
    }
    else if (strcmp(entry->kind, "function") == 0 || strcmp(entry->kind, "program") == 0) {
        faulty = true;
        m_error_printer.print(NonVariableSymbolError(p_variable_ref.getLocation(), p_variable_ref.getNameCString()));
        return;
    }
    if (!p_variable_ref.check_index_valid())
        return;

    std::vector<const char*> dim = p_variable_ref.getDimensions();
    for (size_t i = 0; i < dim.size(); ++i) {
        if (i < entry->type->getDimensions().size() && entry->type->getDimensions()[i] <= 0)
            return;
        const char* dim_str = dim[i];
        for (int j = 0; j < static_cast<int>(strlen(dim_str)); ++j) {
            if (!isdigit(dim_str[j]) && j) {
                faulty = true;
                m_error_printer.print(NonIntegerArrayIndexError(p_variable_ref.getChildLocation(i)));
                return;
            }
            else if (j == 0 && dim_str[j] != '-' && !isdigit(dim_str[j])) {
                faulty = true;
                m_error_printer.print(NonIntegerArrayIndexError(p_variable_ref.getChildLocation(i)));
                return;
            }
        }
    }
    if (entry->type->getDimensions().size() < dim.size()) {
        faulty = true;
        m_error_printer.print(OverArraySubscriptError(p_variable_ref.getLocation(), p_variable_ref.getNameCString()));
        return;
    }

    PType reference = *entry->type;
    std::shared_ptr<PType> type = std::make_shared<PType>(reference);
    type->pop_dimensions(dim.size());
    p_variable_ref.set_type(type);
    return;
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_assignment.visitChildNodes(*this);

    VariableReferenceNode* variable = p_assignment.getLvalue();
    ExpressionNode* expression = p_assignment.getExpr();

    if (variable->getType() == NULL)
        return;
    if (variable->getType()->getDimensions().size() > 0) {
        m_error_printer.print(AssignWithArrayTypeError(variable->getLocation()));
        faulty = true;
        return;
    }

    SymbolEntry* entry = symbol_manager.lookup_globalSymbol(variable->getNameCString());
    if (strcmp(entry->kind, "constant") == 0) {
        faulty = true;
        m_error_printer.print(AssignToConstantError(variable->getLocation(), variable->getNameCString()));
        return;
    }
    if (entry->level < symbol_manager.getLevel() && strcmp(entry->kind, "loop_var") == 0) {
        faulty = true;
        m_error_printer.print(AssignToLoopVarError(variable->getLocation()));
        return;
    }
    if (expression->getType() == NULL)
        return;
    if (expression->getType()->getDimensions().size() > 0) {
        m_error_printer.print(AssignWithArrayTypeError(expression->getLocation()));
        faulty = true;
        return;
    }
    if (strcmp(entry->type->getPTypeCString(), expression->getType()->getPTypeCString()) != 0) {
        if(strcmp(entry->type->getPTypeCString(), "integer") == 0 && strcmp(expression->getType()->getPTypeCString(), "real") == 0)
            return;
        else if(strcmp(expression->getType()->getPTypeCString(), "integer") == 0 && strcmp(entry->type->getPTypeCString(), "real") == 0)
            return;
        faulty = true;
        m_error_printer.print(IncompatibleAssignmentError(p_assignment.getLocation(), variable->getType().get(), expression->getType().get()));
        return;
    }

}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_read.visitChildNodes(*this);
    VariableReferenceNode* variable = p_read.getTarget();
    if (variable->getType() == NULL)
        return;

    SymbolEntry* entry = symbol_manager.lookup_globalSymbol(variable->getNameCString());
    PType* type = entry->type;
    if (!type->scalar_or_not()) {
        faulty = true;
        m_error_printer.print(ReadToNonScalarTypeError(variable->getLocation()));
    }
    if (strcmp(entry->kind, "constant") == 0 || strcmp(entry->kind, "loop_var") == 0) {
        faulty = true;
        m_error_printer.print(ReadToConstantOrLoopVarError(variable->getLocation()));
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);

    ExpressionNode* expression = p_if.getExpression();
    if (expression->getType() == NULL)
        return;
    if (expression->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType) {
        faulty = true;
        m_error_printer.print(NonBooleanConditionError(expression->getLocation()));
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);

    ExpressionNode* expression = p_while.getExpression();
    if (expression->getType() == NULL)
        return;
    if (expression->getType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kBoolType) {
        faulty = true;
        m_error_printer.print(NonBooleanConditionError(expression->getLocation()));
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    int previous_level = loop_var_level;
    symbol_manager.pushScope(new SymbolTable(symbol_manager.getTopType()));

    p_for.visitChildNodes(*this);

    if (!p_for.check_bound()) {
        faulty = true;
        m_error_printer.print(NonIncrementalLoopVariableError(p_for.getLocation()));
    }

    symbol_manager.printCurrentScope();
    symbol_manager.popScope();

    int vars_to_remove = loop_var_level - previous_level;
    while (vars_to_remove > 0) {
        loop_vars.pop_back();
        --vars_to_remove;
    }

    loop_var_level = previous_level;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_return.visitChildNodes(*this);

    ExpressionNode* expression = p_return.getReturnValue();
    if (symbol_manager.getTopType()->getPrimitiveType() == PType::PrimitiveTypeEnum::kVoidType) {
        faulty = true;
        m_error_printer.print(ReturnFromVoidError(p_return.getLocation()));
    }
    else {
        if (expression->getType() == NULL)
            return;
        else {
            if (strcmp(symbol_manager.getTopType()->getPTypeCString(), expression->getType()->getPTypeCString()) != 0) {
                if (strcmp(expression->getType()->getPTypeCString(), "integer") == 0 && strcmp(symbol_manager.getTopType()->getPTypeCString(), "real") == 0)
                    return;
                faulty = true;
                m_error_printer.print(IncompatibleReturnTypeError(p_return.getChildLocation(), symbol_manager.getTopType(), expression->getType().get()));
            }
        }
    }
}

void SemanticAnalyzer::print_symbol_table(bool opt_sbt) {
    symbol_manager.set_print_table(opt_sbt);
}

void SemanticAnalyzer::set_source(char* Source[]) {
    source = Source;
}