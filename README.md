Compiler Construction Project:

Authors: Alexander Krawczyk, Michael Sgoff

Build using: Flex v2.6.4-6, Bison v3.0.4
Compiler tested and used during development: gcc v7.4.0 (on Ubuntu 7.4.0-1ubuntu1~18.04.1)

Structure:

pascal.l - FLEX Tokenizer
pascal.y - BISON Parser (LALR(1) Parsing)
ast_structs.c & .h - Data structures for AST
symtab.c & .h - Symbol & Symbol table data structure
semantic.c & .h - Symbol table, type checking (semantic validation)
translate.c & .h - Translation into LLVM IR
utility.h - Some utility functions

Usage of program is as follows:
Build using: make or make pascal
Clean up: make clean
Usage: ./pascal "pascalprog.pas"

Outputs: pascal.ll as a translated .ll file

