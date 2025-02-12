# Compiler Construction - finals project
Partial implementation of a front-end compiler written in C to generate LLVM machine code for a subset of the PASCAL programming language.

Authors: Alexander Krawczyk, Michael Sgoff

This build uses Flex 2.6.4-6 and Bison 3.0.4. Developed and tested on a machine running Ubuntu 18.04.1 and the gcc 7.4.0 compiler.

### Project structure:
`pascal.l`         - FLEX Tokenizer \
`pascal.y`         - BISON Parser (LALR(1) Parsing) \
`ast_structs.c`    - Data structures for the AST \
`symtab.c`         - Symbol & Symbol table data structure \
`semantic.c`       - Symbol table, type checking (semantic validation) \
`translate.c`      - Translation into LLVM IR \
`utility.h`        - Some utility functions

### Using the program:
Build with `make`, clean up with `make clean` and execute by sourcing `./pascal "pascalprog.pas"`, specifying the path to the the source file to compile. The translated output is written to a file named `pascal.ll`.

### TODOs:
* FOR loops
* Debug operator expression
* Test the use of vars
* Create some sample files for testing
* Fix file writer bug
