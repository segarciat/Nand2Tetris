# Chapter 11: Compiler II: Code Generation

The objective of this project was to extend the compiler that I started creating in the previous project to a full-scale Jack compiler. Instead of generating XML code representing the program's structure, it generates VM code that can then be translated to Hack assembly by the VM translator built in a previous project.

## Context

To support the morph into a full-scale compiler, a symbol table abstraction had to be created maintain variable name labels. The `SymbolTable` keeps track of the _kind_ (varible scope like `static`, `field`, `argument`, `local`), its type (`int`, `char`, `boolean`), and its index (say, `0` for first argument to a function, or `1` for second class `field`).

Moreover, the instead of writing XML, the engine now relies on a `VMWriter` to write VM commands to the output file stream.
