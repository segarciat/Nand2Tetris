# Chapter 7: Virtual Machine I: Stack Arithmetic

The objective of this project was to begin the construction of the VM translator, focusing on the implementation of stack arithmetic and memory access commands.

## Context

The culmination of the software hierarchy of the Hack platform is the construction of a Java-like programming language called _Jack_. Similar to Java's JVM and C#'s CLR, the Jack language compiles into an intermediate VM language. The VM translator in this project is responsible for translating VM commands into assembly, making for a two-step translation. The assembler built in the previous chapter project can then be used to translate commands to binary.

The VM is based on a _stack machine model_, where operands and results of operands are put on a stack. Furthermore, the VM relies on several virtual memory segments: `argument`, `local`, `static`, `constant`, `this`, `that`, `pointer`, and `temp`. Each of these holds a base address for a segment, and a stack pointer `SP` is used to manage the stack. VM commands generally have the form:

```
// Add the value in segment location offset by index to the top of the stack
push segment index

// Move top stack value to segment location offset by index
pop segment index

// Typical arithmetic and logical commands (binary or unary)
add, sub, not, neg, not...
```

## Implementation

The VM translator that I have built is an implementation of the proposed API in the project guidelines. The program takes one command-line argument which may be a VM file or a directory containing VM files. The output is a single assembly file whose commands were translated from all VM files presented to it.
