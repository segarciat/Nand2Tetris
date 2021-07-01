# Chapter 10: Compiler I: Syntax Analysis

The objective of this project was to build a syntax analyzer that parses Jack Programs. The output of the analyzer is an XML file that attempts to uncover the semantics of a Jack program based on its grammar rules.

## Context

As mentioned in previous projects, the Jack programming language is a simple object-oriented language with no support for inheritance. The convention is that each Jack source file has a `.jack` extension and contains a class definition with name being the same as the filename. The goal is to translate a `.jack` source file to a `.vm` file. From there, the VMTranslator and Assembler built in previous chapters complete the translation to Hack binary codes.

The Jack language has quirky keywords such as `do` for calling subroutines when we are not interested in the return value, and `let` for assigning a value to a variable. These were included in the language to simplify the development of compilers for the language, as I did on this project. Actually, the project in this chapter outputs an `.xml` that preserves the structure of the parsed program. It works in two steps:

1. A _tokenizer_ reads the `.jack` source file and parses it into its terminal elemenets (tokens).
2. The _analyzer_ then makes use of an engine which is fed the output of the tokenizer to attempt to generate compiled output. In this project, it emits the parsed structure of the tokenizer in an XML file.

### Implementation

The version of the compiler I have built takes in a single command-line argument; namely, the path to a jack source file or a director containing source files. It outputs an XML file corresponding to each source file.
