# Chapter 6: Assembler

The objective of this project was the construction of Hack's assembler, which translates programs written in Hack assembly language to binary.

The assembler consists of three main modules:

- _Parser_: The `Parser` is responsible for reading in a Hack assembly program and parsing each command in the file. It decides whether the command is an A-command (addressing), a C-command (compute) or an L-Command (pseudo command, for labels). Its interface also provides the component(s) of the relevant command.

- _Code_: The `Code` module translates Hack assembly
  mnemonics that correspond to C-commands to binary codes.

* _SymbolTable_: The `SymbolTable` module is a wrap for a hashmap that keeps track of the ROM address for labels used for jump commands, as well as variable label addresses allocated in RAM.

The `Main` module drives the overall program. It goes through all Hack assembly files with `.asm` extension provided as command-line arguments and generates a `.hack` file for each. To do so, it passes through each `.asm` file twice: once to populate the symbol table with labels, and another to generate the machine code.
