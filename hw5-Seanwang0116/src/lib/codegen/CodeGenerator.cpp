#include "AST/CompoundStatement.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "codegen/CodeGenerator.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             std::unordered_map<SemanticAnalyzer::AstNodeAddr,
                                                      SymbolManager::Table>
                                 &&p_symbol_table_of_scoping_nodes)
    : m_symbol_manager(false /* no dump */),
      m_source_file_path(source_file_name),
      m_symbol_table_of_scoping_nodes(std::move(p_symbol_table_of_scoping_nodes)) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind('/');
    auto dot_pos = source_file_name.rfind('.');

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");

    check_main = false;
    check_global_constant = false;
    local_addr = 8;
    num_of_param = 0;
    check_lvalue = false;
    check_for = false;
    current_label = 0;
    check_head = false;
    label_num = 0;
    check_while = false;
    check_branch = false;
    check_else = false;
    check_if = false;
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::Push_addrStack(const std::string p_name){ addr_stack[p_name].push(local_addr); }

void CodeGenerator::Pop_addrStack(const std::string p_name){
    if (!addr_stack[p_name].empty())
        addr_stack[p_name].pop();
    else
        addr_stack.erase(p_name);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n";

    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the scope for looking up the symbol entry.
    // Hint: Use m_symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_program)));

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);
    
    check_main = true;
    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    constexpr const char*const riscv_assembly_function_epilogue =
        "   lw ra, 124(sp)\n"
        "   lw s0, 120(sp)\n"
        "   addi sp, sp, 128\n"
        "   jr ra\n"
        "   .size main, .-main\n\n";
    
    dumpInstructions(m_output_file.get(), riscv_assembly_function_epilogue);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    const SymbolEntry *symbol_entry = m_symbol_manager.lookup(p_variable.getName());
    if (symbol_entry == NULL)
        return;
    
    switch (symbol_entry->getKind()) {
        case SymbolEntry::KindEnum::kVariableKind: {
            if (symbol_entry->getLevel() == 0) {
                constexpr const char *const riscv_assembly_file_global_variable =
                    ".comm %s, 4, 4    # emit object '%s' to .bss section with size = 4, align = 4\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_file_global_variable, p_variable.getNameCString(), p_variable.getNameCString());
            } else {
                Push_addrStack(p_variable.getName());
                local_addr += 4;
            }
            break;
        }
        case SymbolEntry::KindEnum::kConstantKind: {
            if (symbol_entry->getLevel() == 0) {
                constexpr const char*const riscv_assembly_global_constant =
                    ".section    .rodata\n"
                    "   .align 2\n"
                    "   .globl %s\n"
                    "   .type %s, @object\n"
                    "%s:\n"
                    "    .word ";
                dumpInstructions(m_output_file.get(), riscv_assembly_global_constant, p_variable.getNameCString(), p_variable.getNameCString(), p_variable.getNameCString(), p_variable.getNameCString());
                check_global_constant = true;
                p_variable.visitChildNodes(*this);
            } else {
                Push_addrStack(symbol_entry->getName());
                local_addr += 4;
                constexpr const char*const riscv_assembly_local_constant =
                    "   addi t0, s0, -%d\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_local_constant, local_addr);
                p_variable.visitChildNodes(*this);
                constexpr const char*const riscv_assembly_local_constant2 =
                    "   lw t0, 0(sp)\n"
                    "   addi sp, sp, 4\n"
                    "   lw t1, 0(sp)\n"
                    "   addi sp, sp, 4\n"
                    "   sw t0, 0(t1)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_local_constant2);
            }
            break;
        }
        case SymbolEntry::KindEnum::kLoopVarKind: {
            Push_addrStack(p_variable.getName());
            local_addr += 4;
            break;
        }
        case SymbolEntry::KindEnum::kParameterKind: {
            Push_addrStack(p_variable.getName());
            constexpr const char* const riscv_assembly_func_parameter = 
                "   sw a%d, -%d(s0)\n";
            constexpr const char* const riscv_assembly_func_parameter_to_t_register = 
                "   sw t%d, -%d(s0)\n";

            if (num_of_param < 8)
                dumpInstructions(m_output_file.get(), riscv_assembly_func_parameter, num_of_param, local_addr + 4);
            else
                dumpInstructions(m_output_file.get(), riscv_assembly_func_parameter_to_t_register, num_of_param - 8, local_addr + 4);
                
            local_addr += 4;
            num_of_param++;
            break;
        }
        default:
            break;
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    if (check_global_constant) {
        constexpr const char*const riscv_assembly_global_constant =
        "%s\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_global_constant, p_constant_value.getConstantValueCString());
        check_global_constant = false;
    }
    else {
        constexpr const char*const riscv_assembly_constant = 
        "   li t0, %s\n"
        "   addi sp, sp, -4\n"
        "   sw t0, 0(sp)\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_constant, p_constant_value.getConstantValueCString());
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_function)));
        
    local_addr = 8;
    check_main = false;
    constexpr const char*const riscv_assembly_function_prologue =
        ".section    .text\n"
        "    .align 2\n"
        "   .globl %s\n"
        "   .type %s, @function\n\n"
        "%s:\n"
        "   addi sp, sp, -128\n"
        "   sw ra, 124(sp)\n"
        "   sw s0, 120(sp)\n"
        "   addi s0, sp, 128\n\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_function_prologue, p_function.getNameCString(), p_function.getNameCString(), p_function.getNameCString());

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_function.getParameters().begin(),
             p_function.getParameters().end(), visit_ast_node);
    p_function.visitBodyChildNodes(*this);

    check_main = true;
    local_addr = 8;
    num_of_param = 0;

    constexpr const char*const riscv_assembly_function_epilogue =
        "   # in the function epilogue\n"
        "   lw ra, 124(sp)      # load return address saved in the current stack\n"
        "   lw s0, 120(sp)      # move frame pointer back to the bottom of the last stack\n"
        "   addi sp, sp, 128    # move stack pointer back to the top of the last stack\n"
        "   jr ra               # jump back to the caller function\n"
        "   .size %s, .-%s\n\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_function_epilogue, p_function.getNameCString(), p_function.getNameCString());

    if (m_symbol_manager.getCurrentTable() != NULL) {
        const auto &entries = m_symbol_manager.getCurrentTable()->getEntries();
        for (auto &entry : entries) {
            Pop_addrStack(entry->getName());
        }
    }

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_compound_statement)));
    
    if (check_main) {
        constexpr const char*const riscv_assembly_main_function =
            ".section    .text\n"
            "    .align 2\n"
            "   .globl main\n"
            "   .type main, @function\n\n"

            "main:\n"
            "   addi sp, sp, -128\n"
            "   sw ra, 124(sp)\n"
            "   sw s0, 120(sp)\n"
            "   addi s0, sp, 128\n\n";

            dumpInstructions(m_output_file.get(), riscv_assembly_main_function);
            check_main = false;
    }
    if (check_if) {
        dumpInstructions(m_output_file.get(), "L%d:\n", current_label);
    }
    else if (check_for) {
        constexpr const char*const riscv_assembly_for =
            "   lw t0, 0(sp)\n"
            "   addi sp, sp, 4\n"
            "   lw t1, 0(sp)\n"
            "   addi sp, sp, 4\n"
            "   bge t1, t0, L%d\n"
            "L%d:\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_for, current_label + 2, current_label + 1);
    }
    else if (check_while)
        dumpInstructions(m_output_file.get(), "L%d:\n", current_label + 1);
    else if (check_else) {
        dumpInstructions(m_output_file.get(), "L%d:\n", current_label + 1);
        check_else = false;
    }

    p_compound_statement.visitChildNodes(*this);

    if (check_if) {
        constexpr const char*const riscv_assembly_if =
            "   j L%d\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_if, current_label + 2);
        check_if = false;
        check_else = true;
    }

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_print = 
        "   lw a0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   jal ra, printInt\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_print);
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_load_register =
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   lw t1, 0(sp)\n"
        "   addi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_load_register);

    switch (p_bin_op.getOp()) {
        case Operator::kPlusOp: {
            constexpr const char*const riscv_assembly_add =
                "   add t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_add);
            }
            break;
        case Operator::kMinusOp: {
            constexpr const char*const riscv_assembly_sub =
                "   sub t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_sub);
            }
            break;
        case Operator::kMultiplyOp: {
            constexpr const char*const riscv_assembly_mul =
                "   mul t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_mul);
            }
            break;
        case Operator::kDivideOp: {
            constexpr const char*const riscv_assembly_div =
                "   div t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_div);
            }
            break;
        case Operator::kModOp:{
            constexpr const char*const riscv_assembly_mod =
                "   rem t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_mod);
            }
            break;
        case Operator::kAndOp:  {
            constexpr const char*const riscv_assembly_and =
                "   and t0, t1, t0n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_and);
            }
            break;
        case Operator::kOrOp:{
            constexpr const char*const riscv_assembly_or =
                "   or t0, t1, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_or);
            }
            break;
        case Operator::kLessOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_less =
                        "   bge t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_less, current_label+1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_less =
                        "   bge t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_less, current_label+2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_less =
                    "   slt t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_less);
            }
            }
            break;
        case Operator::kLessOrEqualOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_less_equal =
                        "   bgt t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_less_equal, current_label + 1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_less_equal =
                        "   bgt t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_less_equal, current_label + 2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_less_equal =
                    "   sle t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_less_equal);
            }
            }
            break;
        case Operator::kGreaterOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_greater =
                        "   ble t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_greater, current_label + 1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_greater =
                        "   ble t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_greater, current_label + 2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_greater =
                    "   sgt t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_greater);
            }
            }
            break;
        case Operator::kGreaterOrEqualOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_greater_equal =
                        "   blt t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_greater_equal, current_label + 1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_greater_equal =
                        "   blt t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_greater_equal, current_label + 2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_greater_equal =
                    "   sge t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_greater_equal);
            }
            }
            break;
        case Operator::kEqualOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_expr_equal =
                        "   bne t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_expr_equal, current_label + 1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_equal =
                        "   bne t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_equal, current_label + 2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_equal =
                    "   seq t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_equal);
            }
            }
            break;
        case Operator::kNotEqualOp: {
            if (check_branch) {
                if (check_if) {
                    constexpr const char*const riscv_assembly_not_equal =
                        "   beq t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_not_equal, current_label + 1);
                }
                else if (check_while) {
                    constexpr const char*const riscv_assembly_not_equal =
                        "   beq t1, t0, L%d\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_not_equal, current_label + 2);
                }
            }
            else {
                constexpr const char*const riscv_assembly_not_equal =
                    "   sne t0, t1, t0\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_not_equal);
            }
            }
            break;
    }
    check_branch = false;
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_unary =
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_unary);
    switch (p_un_op.getOp()) {
        case Operator::kNegOp: {
            constexpr const char*const riscv_assembly_neg =
                "   neg t0, t0\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_neg);
            }
            break;
        case Operator::kNotOp: {
            constexpr const char*const riscv_assembly_not =
                "   seq t0, t0, zero\n"
                "   addi sp, sp, -4\n"
                "   sw t0, 0(sp)\n";
            dumpInstructions(m_output_file.get(), riscv_assembly_not);

            if (check_branch) {
                constexpr const char*const riscv_assembly_not_branch=
                "   lw t1, 0(sp)\n"
                "   addi sp, sp, 4\n"
                "   li t0, 0\n"
                "   beq t1, t0, L%d\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_not_branch, current_label + 1);
                check_branch = false;
            }
            }
            break;
    }
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);

    const auto &args = p_func_invocation.getArguments();

    constexpr const char*const riscv_assembly_func_invocation_load_register =
        "   lw a%d, 0(sp)\n"
        "   addi sp, sp, 4\n";
    constexpr const char* const riscv_load_extra_register =
        "   lw t%d, 0(sp)\n"
        "   addi sp, sp, 4\n";

    for (size_t i = 0; i < args.size(); i++) {
        if (i < 8) {
            dumpInstructions(m_output_file.get(), riscv_assembly_func_invocation_load_register, i);
        } else {
            int extra_register_index = i - 8;
            dumpInstructions(m_output_file.get(), riscv_load_extra_register, extra_register_index);
        }
    }
      
    const char* riscv_assembly_push =
        "jal ra, %s\n"
        "mv t0, a0\n"
        "addi sp, sp, -4\n"
        "sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_push, p_func_invocation.getNameCString());
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    const SymbolEntry *symbol_entry = m_symbol_manager.lookup(p_variable_ref.getName());
    if (symbol_entry == NULL)
        return;
    
    if (symbol_entry->getLevel() == 0) {
        switch (symbol_entry->getKind()) {
            case SymbolEntry::KindEnum::kVariableKind: {
                if (check_lvalue) {
                    check_lvalue = false;
                    constexpr const char*const riscv_assembly_glval_reference =
                        "   la t0, %s\n"
                        "   addi sp, sp, -4\n"
                        "   sw t0, 0(sp)\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_glval_reference, p_variable_ref.getNameCString());
                } else {
                    constexpr const char*const riscv_assembly_grval_reference =
                        "   la t0, %s\n"
                        "   lw t1, 0(t0)\n"
                        "   mv t0, t1\n"
                        "   addi sp, sp, -4\n"
                        "   sw t0, 0(sp)\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_grval_reference, p_variable_ref.getNameCString());
                }
                break;
            }
            case SymbolEntry::KindEnum::kConstantKind: {
                constexpr const char*const riscv_assembly_global_constant_reference =
                    "   la t0, %s\n"
                    "   lw t1, 0(t0)\n"
                    "   mv t0, t1\n"
                    "   addi sp, sp, -4\n"
                    "   sw t0, 0(sp)\n";
                dumpInstructions(m_output_file.get(), riscv_assembly_global_constant_reference, p_variable_ref.getNameCString());
                break;
            }
            default:
                break;
        }
    } else {
        switch (symbol_entry->getKind()) {
            case SymbolEntry::KindEnum::kVariableKind:
            case SymbolEntry::KindEnum::kParameterKind:
            case SymbolEntry::KindEnum::kLoopVarKind:
            case SymbolEntry::KindEnum::kConstantKind: {
                int addr = addr_stack[symbol_entry->getName()].top();
                if (check_lvalue) {
                    check_lvalue = false;
                    constexpr const char*const riscv_assembly_llvalue_reference =
                        "   addi t0, s0, -%d\n"
                        "   addi sp, sp, -4\n"
                        "   sw t0, 0(sp)\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_llvalue_reference, addr + 4);
                } else {
                    constexpr const char*const riscv_assembly_lrvalue_reference =
                        "   lw t0, -%d(s0)\n"
                        "   addi sp, sp, -4\n"
                        "   sw t0, 0(sp)\n";
                    dumpInstructions(m_output_file.get(), riscv_assembly_lrvalue_reference, addr + 4);
                }
                break;
            }
            default:
                break;
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    check_lvalue = true;
    p_assignment.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_assignment =
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   lw t1, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   sw t0, 0(t1)\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_assignment);

    if (check_head) {
        check_head = false;
        int addr = addr_stack[p_assignment.getLvalue().getName()].top();
        constexpr const char*const riscv_assembly_for_assign=
            "L%d:\n"
            "   lw t0, -%d(s0)\n"
            "   addi sp, sp, -4\n"
            "   sw t0, 0(sp)\n";
        dumpInstructions(m_output_file.get(), riscv_assembly_for_assign, current_label, addr + 4);
    }
}

void CodeGenerator::visit(ReadNode &p_read) {
    check_lvalue = true;
    p_read.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_read =
        "   jal ra, readInt\n"
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   sw a0, 0(t0)\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_read);
}

void CodeGenerator::visit(IfNode &p_if) {
    check_if = true;
    check_else = false;
    check_branch = true;
    label_base.push(label_num);
    label_num += 3;
    current_label = label_base.top();

    const_cast<ExpressionNode&>(p_if.getCondition()).accept(*this);
    dumpInstructions(m_output_file.get(), "beqz t0, L%d\n", current_label + 1);
    if (p_if.getBody()) {
        const_cast<CompoundStatementNode*>(p_if.getBody())->accept(*this);
    }
    dumpInstructions(m_output_file.get(), "j L%d\n", current_label + 2);

    dumpInstructions(m_output_file.get(), "L%d:\n", current_label + 1);
    if (p_if.getElseBody()) {
        check_else = true;
        const_cast<CompoundStatementNode*>(p_if.getElseBody())->accept(*this);
    }

    dumpInstructions(m_output_file.get(), "L%d:\n", current_label + 2);
    label_base.pop();
    if (!label_base.empty())
        current_label = label_base.top();
    
    check_if = false;
    check_else = false;
}

void CodeGenerator::visit(WhileNode &p_while) {
    label_base.push(label_num);
    label_num += 3;
    current_label = label_base.top();
    constexpr const char*const riscv_assembly_while =
        "L%d:\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_while, current_label);
    check_while = true;
    check_branch = true;
    p_while.visitChildNodes(*this);
    check_while = false;
    constexpr const char*const riscv_assembly_while2 =
        "   j L%d\n"
        "L%d:\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_while2, current_label, current_label + 2);
    label_base.pop();
    if (!label_base.empty())
        current_label = label_base.top();
}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_for)));
    label_base.push(label_num);
    label_num += 3;
    current_label = label_base.top();
    check_for = true;
    check_head = true;

    p_for.visitChildNodes(*this);
    check_for = false;

    int addr = addr_stack[p_for.getLoopVarName()].top();
    constexpr const char*const riscv_assembly_for =
        "   addi t0, s0, -%d\n"
        "   addi sp, sp, -4\n"
        "   sw t0, 0(sp)\n"
        "   lw t0, -%d(s0)\n"
        "   addi sp, sp, -4\n"
        "   sw t0, 0(sp)\n"
        "   li t0, 1\n"
        "   addi sp, sp, -4\n"
        "   sw t0, 0(sp)\n"
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   lw t1, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   add t0, t1, t0\n"
        "   addi sp, sp, -4\n"
        "   sw t0, 0(sp)\n"
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   lw t1, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   sw t0, 0(t1)\n"
        "   j L%d\n"
        "L%d:\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_for, addr + 4, addr + 4, current_label, current_label + 2);
    label_base.pop();
    if (!label_base.empty())
        current_label = label_base.top();

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    constexpr const char*const riscv_assembly_return =
        "   lw t0, 0(sp)\n"
        "   addi sp, sp, 4\n"
        "   mv a0, t0\n";
    dumpInstructions(m_output_file.get(), riscv_assembly_return);
}