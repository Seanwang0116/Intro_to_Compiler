%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}
%token KWRETURN
%token KWEND KWBEGIN 
%token KWPRINT KWREAD KWIF KWELSE KWTHEN KWWHILE KWDO KWFOR KWTO KWOF KWDEF
%token KWVAR KWARRAY KWINTEGER KWBOOLEAN KWREAL KWSTRING
%token INTEGER KWTRUE KWFALSE OCT_INTEGER FLOAT SCIENTIFIC STRING
%token COLON_EQUAL
%token LEFT_BRACKET RIGHT_BRACKET
%token SEMICOLON COLON COMMA
%token ID

%token AND OR NOT
%token SMALL_THAN SMALL_EQUAL SMALL_BIG BIG_EQUAL BIG_THAN EQUAL
%left MINUS
%left PLUS
%left DIVI MOD
%left MULTI
%token LEFT_PARENTHESE RIGHT_PARENTHESE

%nonassoc UNARY_MINUS

%start ProgramName

%%

ProgramName: ID SEMICOLON
             zero_more_declaration
             zero_more_function
             compound_statement
             KWEND
             ;

/* function */
zero_more_function: function_list
                  |
                  ;

function_list: function_declaration
             | function_definition
             | function_list function_declaration
             | function_list function_definition
             ;

function_header: ID LEFT_PARENTHESE opt_formal_augument RIGHT_PARENTHESE COLON scalar_type
               | ID LEFT_PARENTHESE opt_formal_augument RIGHT_PARENTHESE
               ;

function_declaration: function_header SEMICOLON
                    ;

function_definition: function_header
                      compound_statement
                      KWEND
                    ;

opt_formal_augument: formal_augument_list
                    |
                    ;

formal_augument_list: formal_augument
                | formal_augument_list SEMICOLON formal_augument
                ;

formal_augument: idetifier_list COLON type
               ;

/* declaration */
zero_more_declaration: declaration_list
                     |
                     ;

declaration_list: variable_declaration
                | constant_declaration
                | declaration_list variable_declaration
                | declaration_list constant_declaration
                ;

variable_declaration: KWVAR idetifier_list COLON type SEMICOLON
                    ;

constant_declaration: KWVAR idetifier_list COLON opt_minus integer_literal SEMICOLON
                    | KWVAR idetifier_list COLON opt_minus real_literal SEMICOLON
                    | KWVAR idetifier_list COLON string_literal SEMICOLON
                    | KWVAR idetifier_list COLON boolean_literal SEMICOLON
                    ;

opt_minus: MINUS
         |
         ;

idetifier_list: ID
              | idetifier_list COMMA ID
              ;

type: scalar_type
    | array_type
    ;

scalar_type: KWINTEGER
           | KWREAL
           | KWSTRING
           | KWBOOLEAN
           ;

array_type: KWARRAY integer_literal KWOF type
          ;

/* statements */
zero_more_statement: statement_list
                   |
                   ;

statement_list: statement
              | statement_list statement
              ;

statement: simple_statement
          | conditional_statement
          | function_call_statement
          | loop_statement
          | return_statement
          | compound_statement
          ;

simple_statement: variable_reference COLON_EQUAL expression SEMICOLON
                | KWPRINT expression SEMICOLON
                | KWREAD variable_reference SEMICOLON
                ;

conditional_statement: KWIF expression KWTHEN
                       compound_statement
                       KWELSE
                       compound_statement
                       KWEND KWIF
                     | KWIF expression KWTHEN
                       compound_statement
                       KWEND KWIF
                     ;

function_call_statement: function_call SEMICOLON
                       ;
loop_statement: KWWHILE expression KWDO
                compound_statement
                KWEND KWDO
              | KWFOR ID COLON_EQUAL integer_literal KWTO integer_literal KWDO
                compound_statement
                KWEND KWDO
              ;

return_statement: KWRETURN expression SEMICOLON
                ;

compound_statement: KWBEGIN
                    zero_more_declaration
                    zero_more_statement
                    KWEND
                  ;

/* expression */
zero_more_expression: expression_list
              |
              ;

expression_list: expression
               | expression_list COMMA expression
               ;

expression: literal_constant
          | variable_reference
          | function_call
          | arithmetic_expression
          ;

literal_constant: integer_literal
                | real_literal
                | string_literal
                | boolean_literal
                ;

integer_literal: OCT_INTEGER
               | INTEGER
               ;

real_literal: FLOAT
            | SCIENTIFIC
            ;

string_literal: STRING
              ;

boolean_literal: KWTRUE
               | KWFALSE
               ;

variable_reference: ID zero_more_array
                  ;

zero_more_array: array_list
               |
               ;

array_list: LEFT_BRACKET expression RIGHT_BRACKET
          | array_list LEFT_BRACKET expression RIGHT_BRACKET
          ;

function_call: ID LEFT_PARENTHESE zero_more_expression RIGHT_PARENTHESE
             ;

arithmetic_expression: MINUS expression %prec UNARY_MINUS
                     | expression binary_operator expression
                     | NOT expression
                     | LEFT_PARENTHESE expression RIGHT_PARENTHESE
                     ;

binary_operator: MULTI
               | DIVI
               | MOD
               | PLUS
               | MINUS
               | SMALL_THAN
               | SMALL_EQUAL
               | SMALL_BIG
               | BIG_EQUAL
               | BIG_THAN
               | EQUAL
               | AND
               | OR
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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}