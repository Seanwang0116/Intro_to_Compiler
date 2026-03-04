# P-Compiler: Full Lifecycle Compiler Implementation

This project is a comprehensive implementation of a compiler for the **P language**. It explores the full journey of code transformation—from raw source text to functional **RISC-V assembly**—through five incremental engineering phases.

---

## Technical Specifications
To build and run this compiler, ensure your environment meets the following requirements:

| Requirement | Details / Version |
| :--- | :--- |
| **Implementation** | `C++` (utilizing STL and Visitor Pattern) |
| **Lexer / Parser** | `Flex` / `Bison` |
| **Target Arch** | `RISC-V` (64-bit) |
| **Simulation** | `Spike` Simulator / `GD32` Development Board |
| **Build System** | `Makefile` |

---

## Development Roadmap

### Phase I: Lexical Analysis (Scanner)
* **Focus**: Tokenizing the source code into manageable units.
* **Challenges**: Implementation of complex regular expressions for scientific notation and nested C-style comments.
* **Key Achievement**: Handled edge cases in string literals and escaped quotes (`""`).

### Phase II: Syntactic Analysis (Parser)
* **Focus**: Defining the LALR(1) grammar for the P language.
* **Key Achievement**: Established formal rules for program units, variable declarations, and nested control statements using Bison.

### Phase III: Abstract Syntax Tree (AST)
* **Focus**: Constructing a structured Intermediate Representation (IR).
* **Key Achievement**: Implemented the **Visitor Pattern** to decouple the AST structure from its logic (traversal/printing), providing a solid foundation for semantic analysis.

### Phase IV: Semantic Analysis
* **Focus**: Enforcing language rules and type safety.
* **Components**:
    * **Symbol Table**: Scope-aware management (Global/Local) for identifiers.
    * **Type Checking**: Validation of expression types, function signatures, and variable initialization.

### Phase V: Code Generation
* **Focus**: Translating the AST into executable RISC-V assembly.
* **Key Achievement**: Implemented a stack-based execution model supporting function calls, loops (For/While), and standard I/O (Read/Print).

---

## Evaluation & Testing
The project ensures reliability through a rigorous testing pipeline:

### Automated Testing
* **Execution**: Run `make test` to trigger the automated test suite.
* **Validation**: Outputs are compared against `sample_solution` using `diff` to ensure 100% logic parity.

### Hardware Integration
* **Board Support**: For the final phase, the generated code can be deployed to the **GD32 Board**.
* **Verification**: Results are displayed directly on the board's LCD, confirming real-world hardware compatibility.

---
