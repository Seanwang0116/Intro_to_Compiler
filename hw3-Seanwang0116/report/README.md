# hw3 report

|      |           |
| ---: | :-------- |
| Name | 王紹安    |
|   ID | 111550116 |

## How much time did you spend on this project

> e.g. 30hr( 6h per day).

## Project overview

> I implemented the vision pattern provided in the template. First i added the yyval in the scanner.l to catch the returned value. Then followed the ast_guideline to construct the AST step by step, from bottom to top( e.g. when I need to construct the `declaration_node`, I have to construct `variable_node`, and further construct `constant_variable_node` before that.).

> Nodes are constructed using different classes based on the specific semantic attributes of each node. For example, there are program nodes, declaration nodes, assignment nodes, etc. However, all of these nodes inherit from a common parent class: `AstNode`. This inheritance structure facilitates the construction of visit operations in the Visitor Pattern. The visitor does not need to know the specific type of the node, but rather assumes that all nodes are of type `AstNode` and requests output information from these nodes. Therefore, the responsibility of the node is to pass its internal information to the visitor. After receiving the information, the visitor performs a pre-order print of the node.

> Finally, the parser construct the AST during parsing, and visited each node to fetch their information.

## What is the hardest you think in this project

> Seeing such a large and complex codebase is definitely a big challenge for me. Understanding the overall architecture and design of the program, especially with the intricate use of the Visitor Pattern and how each node in the Abstract Syntax Tree (AST) functions, requires a significant amount of effort. Each node has its own specific properties and methods, and I need to understand how these different nodes — like program nodes, declaration nodes, assignment nodes, and so on — interact with one another.

> One key aspect of this challenge is the need to establish the necessary information for each node to ensure that everything ties together properly. The nodes themselves rely on each other to create a working system. For example, some nodes will contain references to other nodes or specific values that need to be passed through the program.

> Additionally, understanding how the visitor operates on these nodes and how each node is responsible for passing its internal information to the visitor adds another layer of complexity. The visitor doesn't need to know about the specifics of each node, but instead can operate on all nodes in a generalized manner. This abstraction is powerful, but it also means that I have to ensure every node is correctly structured and provides the right information for the visitor to work with.

## Feedback to T.A.s

> Thanks for this assignment. It has not only helped me become more familiar with AST, but also allowed me to learn about the Visitor Pattern.
