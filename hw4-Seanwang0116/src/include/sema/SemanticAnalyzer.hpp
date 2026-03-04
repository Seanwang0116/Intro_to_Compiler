#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "sema/ErrorPrinter.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "AST/PType.hpp"

#include <vector>
#include <stack>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>

class SymbolEntry {
public:
    const char* name; 
    const char* kind; 
    int level;
    PType* type;
    const char* attribute;
};

class SymbolTable {
public:
    SymbolTable() = default;
    SymbolTable(PType* return_type) : return_type(return_type) {};
    void addSymbol(const char* name,
                   const char* kind,
                   int level,
                   PType* type,
                   const char* attribute);
    bool lookupSymbol(const char* name);
    SymbolEntry* lookup_localSymbol(const char* name);
    // keep the order by simply using vector
    std::vector<SymbolEntry> entries;
    PType* return_type = NULL;
};

class SymbolManager {
public:
    SymbolManager() = default;
    void pushScope(SymbolTable *new_scope);
    void popScope();
    void printCurrentScope();
    void set_print_table(bool opt_sbt);
    void insertSymbol(const char* name,
                    const char* kind,
                    int level,
                    PType* type,
                    const char* attribute);
    bool lookupSymbol(const char* name);
    SymbolEntry* lookup_globalSymbol(const char* name);
    PType* getTopType() {return tables.top()->return_type;};
    int getLevel() {return tables.size() - 1; };

private:
    std::stack<SymbolTable *> tables;
    bool print_or_not = true;
};

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    ErrorPrinter m_error_printer{stderr};
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    SymbolManager symbol_manager;
    char** source;
    std::vector<std::string> loop_vars;
    int loop_var_level = 0;
    bool faulty = false;

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    void print_symbol_table(bool opt_sbt);
    void set_source(char* source[]);
};

#endif
