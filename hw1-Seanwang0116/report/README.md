# hw1 report

| Field | Value     |
| ----: | :-------- |
|  Name | 王紹安    |
|    ID | 111550116 |

## How much time did you spend on this project

 6 hours.

## Project overview

 **Token Definitions** :

* The lexer defines various tokens such as identifiers, integers, floating-point numbers, strings, comments, delimiters, operators, and reserved words using regular expressions.
* Each token type is assigned a corresponding action (i.e., what to do when the token is matched).

 **Token Actions** :

* For each token pattern defined, actions are specified in braces** **`{}`. For instance:
  * When an identifier is matched, it calls** **`listLiteral("id", yytext);` to print the identifier.
  * For comments, the action is to simply match them without any output.
* Special handling is included for string literals to manage escaped quotes (`""`).

 **Whitespace and Newline Handling** :

* Whitespace (`[ \t]`) is ignored, while newlines (`\n`) reset the column count and update the current line.

## What is the hardest you think in this project

It was a huge challenge for me to write the regular expression(exspecially scientific and C_comment) so I spend much time to look up how to write these. And I also spend some time on solving string , need to consider some edge cases.

## Feedback to T.A.s

Very helpful assignment !
