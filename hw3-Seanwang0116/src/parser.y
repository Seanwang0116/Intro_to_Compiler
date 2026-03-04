%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"
#include "AST/AstDumper.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

bool check_boolean = false;
Ptype *type_of_constant;
std::vector<int> *size_of_array = NULL;
%}

// This guarantees that headers do not conflict when included together.
%define api.token.prefix {TOK_}

%code requires {
    class AstNode;
    class ProgramNode;
    class CompoundStatementNode;
    class DeclNode;
    class VariableNode;
    class ConstantValueNode;
    class FunctionNode;
    class ExpressionNode;

    class PrintNode;
    class FunctionInvocationNode;
    class VariableReferenceNode;
    class AssignmentNode;
    class ReadNode;
    class IfNode;
    class WhileNode;
    class ForNode;
    class ReturnNode;

    class BinaryOperatorNode;
    class UnaryOperatorNode;

    #include <string>
    #include <vector>
    #include <AST/adder.hpp>
}

    /* For yylval */
%union {
    /* basic semantic value */
    int int_value;
    float float_value;
    char* string_value;
    bool bool_value;
    char *identifier;

    AstNode *node;
    std::vector<AstNode *> *ats_list;
    ProgramNode *program_node;
    CompoundStatementNode *compound_statment_node;

    DeclNode *declaration_node;
    std::vector<DeclNode* > *declaration_list;
    FunctionNode *function_node;
    std::vector<FunctionNode* > *function_list;
    FunctionInvocationNode *function_invocation_node;
    
    ForNode *for_node;
    IfNode *if_node;
    ReturnNode *return_node;
    VariableReferenceNode *variable_reference_node;
    WhileNode *while_node;

    ExpressionNode *expression_node;
    std::vector<ExpressionNode *> *expression_list;

    ConstantValueNode *constant_value_node;

    Ptype *pytpe;

    std::vector<VariableNode *> *id_list;
    std::string *string_boolean;
};

%type <program_node> Program
%type <identifier> ProgramName ID FunctionName
%type <compound_statment_node> CompoundStatement ElseOrNot
%type <ats_list> StatementList Statements
%type <declaration_list> DeclarationList Declarations FormalArgs FormalArgList
%type <declaration_node> Declaration FormalArg
%type <function_list> FunctionList Functions
%type <function_node> Function FunctionDeclaration FunctionDefinition
%type <function_invocation_node> FunctionInvocation
%type <variable_reference_node> VariableReference
%type <pytpe> ScalarType Type ArrType ReturnType
%type <node> ProgramUnit Statement Simple
%type <node> ArrDecl;
%type <id_list> IdList
%type <constant_value_node> LiteralConstant IntegerAndReal
%type <bool_value> NegOrNot
%type <string_boolean> StringAndBoolean
%type <expression_node> Expression
%type <expression_list> ExpressionList Expressions ArrRefs ArrRefList
%type <if_node> Condition
%type <while_node> While
%type <for_node> For
%type <return_node> Return

    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
    /* TODO: specify the precedence of the following operators */
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token <int_value> INT_LITERAL
%token <float_value> REAL_LITERAL
%token <string_value> STRING_LITERAL

%%

ProgramUnit:
    Program
    |
    Function
;

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, $3, $4, $5);

        free($1);
    }
;

ProgramName:
    ID {
        $$ = $1;
    }
;

DeclarationList:
    Epsilon {
        $$ = NULL;
    }
    |
    Declarations {
        $$ = $1;
    }
;

Declarations:
    Declaration {
        $$ = new std::vector<DeclNode *>();
        $$->emplace_back($1);
    }
    |
    Declarations Declaration {
        $1->emplace_back($2);
        $$ = $1;
    }
;

FunctionList:
    Epsilon {
        $$ = NULL;
    }
    |
    Functions {
        $$ = $1;
    }
;

Functions:
    Function {
        $$ = new std::vector<FunctionNode *>();
        $$->emplace_back($1);
    }
    |
    Functions Function {
        $1->emplace_back($2);
        $$ = $1;
    }
;

Function:
    FunctionDeclaration
    |
    FunctionDefinition
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, NULL);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6);
    }
;

FunctionName:
    ID {
        $$ = $1;
    }
;

FormalArgList:
    Epsilon {
        $$ = NULL;
    }
    |
    FormalArgs {
        $$ = $1;
    }
;

FormalArgs:
    FormalArg {
        $$ = new std::vector<DeclNode *>();
        $$->emplace_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg {
        $1->emplace_back($3);
        $$ = $1;
    }
;

FormalArg:
    IdList COLON Type {
        $$ = new DeclNode(@1.first_line, @1.first_column, $1, $3, size_of_array);
        delete size_of_array;
        size_of_array = NULL;
    }
;

IdList:
    ID {
        $$ = new std::vector<VariableNode *>{ new VariableNode(@1.first_line, @1.first_column, $1) };
    }
    |
    IdList COMMA ID {
        $1->emplace_back(new VariableNode(@3.first_line, @3.first_column, $3));
        $$ = $1;
    }
;

ReturnType:
    COLON ScalarType {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = new Ptype(Ptype::VOID);
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON Type SEMICOLON {
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, $4, size_of_array);
        delete size_of_array;
        size_of_array = NULL;
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        $$ = new DeclNode(@1.first_line, @1.first_column, $2, type_of_constant, $4);
        delete type_of_constant;
    }
;

Type:
    ScalarType {
        $$ = $1;
    }
    |
    ArrType {
        $$ = $1;
    }
;

ScalarType:
    INTEGER {
        $$ = new Ptype(Ptype::INT);
    }
    |
    REAL {
        $$ = new Ptype(Ptype::REAL);
    }
    |
    STRING {
        $$ = new Ptype(Ptype::STRING);
    }
    |
    BOOLEAN {
        $$ = new Ptype(Ptype::BOOLEAN);
    }
;

ArrType:
    ArrDecl ScalarType {
        $$ = $2;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        size_of_array = new std::vector<int>{ $2 };
        $$ = NULL;
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        size_of_array->emplace_back($3);
        $$ = NULL;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        type_of_constant = new Ptype(Ptype::INT);
        $$ = new ConstantValueNode(
            $1 ? @1.first_line : @2.first_line, 
            $1 ? @1.first_column : @2.first_column, 
            $1 ? -$2 : $2
        );
    }
    |
    NegOrNot REAL_LITERAL {
        type_of_constant = new Ptype(Ptype::REAL);
        $$ = new ConstantValueNode(
            $1 ? @1.first_line : @2.first_line, 
            $1 ? @1.first_column : @2.first_column, 
            $1 ? -$2 : $2
        );
    }
    |
    StringAndBoolean {
        type_of_constant = check_boolean ? new Ptype(Ptype::BOOLEAN) : new Ptype(Ptype::STRING);
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);
        check_boolean = false;
    }
;

NegOrNot:
    Epsilon {
        $$ = false;
    }
    |
    MINUS %prec UNARY_MINUS{
        $$ = true;
    }
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = new std::string( $1 );
    }
    |
    TRUE {
        $$ = new std::string("true");
        check_boolean = true;
    }
    |
    FALSE {
        $$ = new std::string("false");
        check_boolean = true;
    }
;

IntegerAndReal:
    INT_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);
    }
    |
    REAL_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, $1);
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = static_cast<AstNode *>($1);
    }
    |
    Simple {
        $$ = $1;
    }
    |
    Condition {
        $$ = static_cast<AstNode *>($1);
    }
    |
    While {
        $$ = static_cast<AstNode *>($1);
    }
    |
    For {
        $$ = static_cast<AstNode *>($1);
    }
    |
    Return {
        $$ = static_cast<AstNode *>($1);
    }
    |
    FunctionCall
;

CompoundStatement:
    BEGIN
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        AssignmentNode* assignNode = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
        $$ = static_cast<AstNode*>(assignNode);
    }
    |
    PRINT Expression SEMICOLON {
        PrintNode* printNode = new PrintNode(@1.first_line, @1.first_column, $2);
        $$ = static_cast<AstNode*>(printNode);
    }
    |
    READ VariableReference SEMICOLON {
        ReadNode* readNode = new ReadNode(@1.first_line, @1.first_column, $2);
        $$ = static_cast<AstNode*>(readNode);
    }
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
    }
;

ArrRefList:
    Epsilon {
        $$ = NULL;
    }
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = new std::vector<ExpressionNode *>{ $2 };
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $1->emplace_back($3);
        $$ = $1;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = NULL;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO {
        VariableNode *varNode = new VariableNode(@2.first_line, @2.first_column, $2);

        std::vector<VariableNode *> varNodes;
        varNodes.push_back(varNode);
        DeclNode *declNode = new DeclNode(@2.first_line, @2.first_column, new std::vector<VariableNode *>(varNodes), new Ptype(Ptype::INT));
        
        VariableReferenceNode *varRefNode = new VariableReferenceNode(@2.first_line, @2.first_column, $2);

        ConstantValueNode *constValueNode = new ConstantValueNode(@4.first_line, @4.first_column, $4);

        AssignmentNode *assignNode = new AssignmentNode(@3.first_line, @3.first_column, varRefNode, static_cast<ExpressionNode*>(constValueNode));

        ConstantValueNode *loopCondNode = new ConstantValueNode(@6.first_line, @6.first_column, $6);

        $$ = new ForNode(@1.first_line, @1.first_column, declNode, assignNode, loopCondNode, $8);
    }
;

Return:
    RETURN Expression SEMICOLON {
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {
        $$ = NULL;
    }
    |
    Expressions {
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = new std::vector<ExpressionNode *>{ $1 };
    }
    |
    Expressions COMMA Expression {
        $1->emplace_back($3);
        $$ = $1;
    }
;

StatementList:
    Epsilon {
        $$ = NULL;
    }
    |
    Statements {
        $$ = $1;
    }
;

Statements:
    Statement {
        $$ = new std::vector<AstNode *>{ $1 };
    }
    |
    Statements Statement {
        $1->emplace_back($2);
        $$ = $1;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        UnaryOperatorNode* unaryOpNode = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2);
        $$ = static_cast<ExpressionNode *>(unaryOpNode);
    }
    |
    Expression MULTIPLY Expression {
        BinaryOperatorNode* multiplyOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3);
        $$ = static_cast<ExpressionNode *>(multiplyOpNode);
    }
    |
    Expression DIVIDE Expression {
        BinaryOperatorNode* divideOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3);
        $$ = static_cast<ExpressionNode *>(divideOpNode);
    }
    |
    Expression MOD Expression {
        BinaryOperatorNode* modOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3);
        $$ = static_cast<ExpressionNode *>(modOpNode);
    }
    |
    Expression PLUS Expression {
        BinaryOperatorNode* plusOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3);
        $$ = static_cast<ExpressionNode *>(plusOpNode);
    }
    |
    Expression MINUS Expression {
        BinaryOperatorNode* minusOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3);
        $$ = static_cast<ExpressionNode *>(minusOpNode);
    }
    |
    Expression LESS Expression {
        BinaryOperatorNode* lessOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3);
        $$ = static_cast<ExpressionNode *>(lessOpNode);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        BinaryOperatorNode* less_equalOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3);
        $$ = static_cast<ExpressionNode *>(less_equalOpNode);
    }
    |
    Expression GREATER Expression {
        BinaryOperatorNode* greaterOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3);
        $$ = static_cast<ExpressionNode *>(greaterOpNode);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        BinaryOperatorNode* greater_equalOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3);
        $$ = static_cast<ExpressionNode *>(greater_equalOpNode);
    }
    |
    Expression EQUAL Expression {
        BinaryOperatorNode* equalOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3);
        $$ = static_cast<ExpressionNode *>(equalOpNode);
    }
    |
    Expression NOT_EQUAL Expression {
        BinaryOperatorNode* not_equalOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3);
        $$ = static_cast<ExpressionNode *>(not_equalOpNode);
    }
    |
    NOT Expression {
        UnaryOperatorNode* notOpNode = new  UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2);
        $$ = static_cast<ExpressionNode *>(notOpNode);
    }
    |
    Expression AND Expression {
        BinaryOperatorNode* andOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3);
        $$ = static_cast<ExpressionNode *>(andOpNode);
    }
    |
    Expression OR Expression {
        BinaryOperatorNode* orOpNode = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3);
        $$ = static_cast<ExpressionNode *>(orOpNode);
    }
    |
    IntegerAndReal {
        $$ = static_cast<ExpressionNode*>( $1 );
    }
    |
    StringAndBoolean {
        $$ = static_cast<ExpressionNode *>(new ConstantValueNode(@1.first_line, @1.first_column, $1));
    }
    |
    VariableReference {
        $$ = static_cast<ExpressionNode *>( $1 );
    }
    |
    FunctionInvocation {
        $$ = static_cast<ExpressionNode *>( $1 );
    }
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    
    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
