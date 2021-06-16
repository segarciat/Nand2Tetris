// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

(MAINLOOP)
  @SCREEN   // Initiate loop variable to base screen address
  D=A
  @addr
  M=D
  @KBD        // Decide whether to clear or blacken pixel
  D=M
  @BLACKEN
  D;JGT
  (CLEAR)     // Clear pixel by setting its value to 0
    @color_val
    M=0
    @COLOR
    0;JMP
  (BLACKEN)
    @color_val
    M=-1      // Blacken pixel by setting its value to -1 (all 1s)
(COLOR)
  @addr       // Terminate loop when addr equals KBD address
  D=M
  @KBD
  D=D-A
  @MAINLOOP
  D;JEQ
  @color_val   // Set color value (0 for clear, -1 for blacken)
  D=M
  @addr        // Color the pixel
  A=M
  M=D
  @addr        // Increment loop variable
  M=M+1
  @COLOR
  0;JMP