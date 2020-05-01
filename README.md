# nand2tetris
Projects for the Nand2Tetris book, which culminated in the construction of the Hack platform, along with the Jack programming language.

## Background
Nand To Tetris, otherwise known as 'The Elements of Computing Systems' is a book aby Naom Nisan and Shimon Schoken with the goal of
taking readers through the construction of a computer system. Over the course of 12 chapters, each with an end-of-chapter project,
I was led to the gradual build-up of the basic hardware platform and software hierarchy. The first five chapters focused on the
construction of the Hack hardware platform, while the rest focused on building a Java-like high level programming language called Jack,
along with its it compiler hierarchy and 'operating system'. The book focused on breadth rather than depth.

## The Hack hardware platform projects
The harware part of the journey was split between five chapters:
1. **Boolean Logic**: Construction of boolean gates based on the Nand gate, such as Multiplexors and Xor gates. Written in Hardware Description Langauge (HDL).
2. **Boolean Arithmetic**: Construction of the Hack ALU chip using HDL.
3. Sequential Logic: Construction of sequental chips for storage, such as registers, RAM chips, and a program computer, in HDL.
4. Machine Language: Low-level programming in the Hack platform. Particularly, writing simple .asm (assembly) programs.
5. Computer Architecture: Construction of Hack, specifically: the CPU chip, the Memory chip, and the Computer chip.

## The software hierarchy projects
The software part of the journey was split between 7 chapters. I chose to write my projects in C++. More details follow:
6. **Assembler**: Construction of an assembler, which takes in Hack assembly files and outputs Hack binary code files.
7. **Virtual Machine I: Stack Arithmetic**: First part of virtual machine backend, which handles stack arithmetic.
8. **Virtual Machine II: Program Flow**: Second part of the virtual machine backend, which handles program flow (if, while, functions).
9. **High-Level Language**: Programming with Jack, a simple yet modern high-level object-based language. I chose to write a simple game
that resembles the dinosaur game you can play in Chrome when your connection is down.
10. **Compiler I: Syntax Analysis**: First part of the construction of Jack compiler, which started with the Tokenizer.
11. **Compiler I: Code Generation**: Second part of the construction of Jack compiler, which consisted of a symbol table, a code writer,
and a compilation engine that made used of these modules. It takes a folder consisting of .jack files and compiles the to .vm files.
12. **Operating System**: Writing of the Jack Operating System (more like standard library) to provide a simple interface for memory
management, input and output, mathematical computations, and others.

## Reflections
I had a blast writing getting through this book and working on this project. It was a lot of work, but it was all worthwhile for the
knowledge I gained. I wholeheartedly recommend this to anyone looking to deep their knowledge of how computers work under the hood.
Hopefully you'll be luckier than me and have someone with whom you're able to discuss the book while you complete it :)

## References
https://www.nand2tetris.org/
