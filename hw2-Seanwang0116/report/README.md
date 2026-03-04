# hw2 report

|      |           |
| ---: | :-------- |
| Name | 王紹安    |
|   ID | 111550116 |

## How much time did you spend on this project

> 8 hours.

## Project overview

> In scanner.l, I named the return value of these tokens in uppercase (such as `KWOF`, `KWVAR`, etc.) follows a common convention in  lexical analyzers . This approach ensures that token names are distinct and easy to differentiate from other code elements, like variables or keywords, which are typically lowercase.
>
> In parser.y, I follow the flow of spec to construct CFG gradually, it can be 
>
>  **Token Definitions :**
>
> * Defines language tokens such as keywords (`KWRETURN`, `KWBEGIN`), data types (`INTEGER`, `FLOAT`), operators (`PLUS`, `MINUS`), and punctuation (`SEMICOLON`, `COMMA`).
> * Specifies operator precedence and associativity using `%left` and `%nonassoc`.
>
> **Program Structure :**
>
> * `ProgramName`: Main rule for program structure, including declarations, functions, and compound statements.
> * `zero_more_function` and `zero_more_declaration`: Handles zero or more functions and declarations.
>
> **Functions :**
>
> * Rules for function declaration and definition (`function_declaration`,`function_definition`), as well as formal arguments.
>
> **Declarations :**
>
> * Handles variable (`variable_declaration`) and constant declarations (`constant_declaration`), specifying types and initialization.
>
> **Statements :**
>
> * Includes simple statements (assignments, `print`, `read`), conditional statements (`if`, `else`), loops (`while`, `for`), and compound statements.
>
> **Expressions :**
>
> * Defines valid expressions like literals, variable references, function calls, and arithmetic expressions (`PLUS`,`MINUS`, logical operators, etc.).

## What is the hardest you think in this project

> When I first see `{ item } denotes zero or more occurrences of the item` , I had no idea how to handle it at all, so I tried to look up lecturenote and accidently find solution is that create a non-empty set, this save my day! Another issue is that I didnt know how to handle unary input, I tried many time but failed so I turned to internet for help and find solution, finally complete this assignment.

## Feedback to T.A.s

> This assignment has improved my understanding of CFG taught in class, and I now have a basic understanding of the syntax of `yacc`. Much thanks to TAs!
