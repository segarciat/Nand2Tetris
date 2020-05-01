// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
      @R2 // Set A = memory address of RAM 2.
      M=0 // R2=0, initializing the result of multiplication. Set RAM[2] = 0
      @i  // i = 0
      M=0 // Initialize loop control variable.
(LOOP)
      @i      // Set A to the memory location of some variable i. Now M is the value at that location.
      D=M     // D=i.
      @R1
      D=D-M   // D=i-RAM[1].
      @END
      D;JGE   // End loop if i==RAM[1].
      @R0
      D=M     // D=RAM[0]
      @R2
      M=M+D   // R2=R2+R0
      @i
      M=M+1   // i=i+1
      @LOOP   // Set A to the address of the beginning of the loop named LOOP.
      0;JMP   // Go to LOOP
(END)
      @END
      0;JMP   // Infinite loop; ends program.
