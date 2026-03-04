# hw5 report

|      |           |
| ---: | :-------- |
| Name | 王紹安    |
|   ID | 111550116 |

## How much time did you spend on this project

> 24hr(6hr per day)

## Project overview

> This code generator is designed for the P programming language. It builds on the implementation of the AST from previous assignments and follows a visitor pattern to traverse the tree in preorder, producing RISC-V assembly code. The generator handles various constructs such as variables, expressions, loops, and conditional statements.

> ### Core Features:

1. **Visitor Pattern for AST Traversal**
   The generator employs a visitor pattern to systematically traverse the AST nodes. Each node type—variables, expressions, loops, and conditions—has specific rules to generate the corresponding assembly code.
2. **Variable and Scope Management**
   To manage variables across different scopes, a specialized data structure,`addr_stack`, is utilized.
   * Each variable's address, accessible within the current scope, is stored at the top of its corresponding stack in `addr_stack[variable_name]`.
   * When exiting a scope, the stack for each variable is popped, ensuring the correct handling of variable lifetimes and addresses.
3. **Expression Evaluation**
   For variables and expressions, the generator focuses on efficiently using registers and correctly addressing stack locations. This ensures that variable data is both stored and retrieved accurately during code execution.
4. **Conditional and Looping Constructs**
   The generator emphasizes proper branching for unary and binary operations in conditions and loops.
   * To manage nested conditions and loops, a label stack (`label_stack`) is used.
   * Each time a new conditional or loop block is entered, a unique label number is pushed onto the stack, and the current label is updated to the top of the stack.
   * Upon exiting a block, the stack is popped, restoring the previous label.

> ### Benefits of the Approach:

* **Scalability** : The use of `addr_stack` and `label_stack` enables seamless handling of nested scopes and constructs.
* **Clarity** : The separation of scope and label management ensures that the generated assembly code is both clear and correct.
* **Modularity** : By following the visitor pattern, the generator can easily adapt to new node types or changes in language semantics.

## What is the hardest you think in this project

> I have trouble in advanced function due to segmentation fault and I have no idea how to fix it.

## Feedback to T.A.s

> I gained invaluable knowledge from this course through hands-on experience in designing a compiler. The assignments were thoughtfully crafted, breaking down key concepts into a semester-long project that built upon each previous step. Despite the countless sleepless nights spent wrestling with complex code, this is a course I’ll never regret taking. The challenges were demanding but ultimately rewarding, providing a deep understanding of compiler design and implementation.
