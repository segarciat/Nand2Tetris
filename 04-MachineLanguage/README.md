# Chapter 4: Machine Language

The objective of this chapter and project was to acquainted with low-level programming in machine language. To do so, I wrote a couple simple programs in Hack's assembly language:

1. _Mult_: This simple assembly program computes the result of multiplying two integers stored in registers `R0` and `R1`, respectively. The result is stored in register `R2` at the end of the program.

2. _Fill_: This program probes for a key press. If a key has been pressed, colors Hack's screen black by setting the values of all bits in its RAM-resident memory map to 1. Otherwise, it clears the screen.

The programs were written in Hack's assembly language, and translated to Hack's machine language by using an _Assembler_ for Hack provided with the project resources (which is to be built in project 6).
