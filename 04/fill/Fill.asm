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

// Put your code here.
      @R2
      M=0     // R2=0, initializing the result of multiplication.
      @i
      M=0     // Initialize loop control variable.
      @256
      D=A
      @R0
      M=D    // Set RAM[0] to 256 (multiplication operand #1).
      @32
      D=A
      @R1
      M=D    // Set RAM[1] to 32 (multiplication operand #2).
(MULTLOOP)
      @i
      D=M     // D=i.
      @R1
      D=D-M   // D=i-RAM[1].
      @PREMAIN
      D;JGE   // End loop if i>=RAM[1].
      @R0
      D=M     // D=RAM[0]
      @R2
      M=M+D   // R2=R2+R0
      @i
      M=M+1   // i=i+1
      @MULTLOOP
      0;JMP   // Go to LOOP
(PREMAIN)
      @SCREEN
      D=A
      @R2
      D=D+M
      @max
      M=D     // Max address for keyboard memory mapping.
(MAINLOOP)
      @SCREEN
      D=A       // D=16384
      @p_word
      M=D       // p_word=16384, first screen location.
      @24576
      D=M     // D=RAM[24576], ASCII value of key pressed or 0.
      @BLACKEN
      D;JGT   // BLACKEN screen on nonzero value.
      @CLEAR
      0;JMP   // Otherwise, clear the screen.
      (BLACKEN)
            @p_word
            A=M     // Set A to current ASCII key address (A=RAM[A])
            M=-1    // With M now pointing to A as above, blacken the word there.
            @p_word
            M=M+1   // Move to next word: M[p_word] == cur_pos + 16
            D=M
            @max
            D=D-M   // ex: 16384 - (16384 - 8192)
            @MAINLOOP
            D;JGE
            @BLACKEN
            0;JMP   // Done with FILL_LOOP if D>=0.
      (CLEAR)
            @p_word
            A=M     // Set A to current address.
            M=0    // Blacken the word.
            @p_word
            M=M+1   // M[p_word] == cur_pos + 16
            D=M
            @max
            D=D-M   // ex: 16384 - (16384 + 8192)
            @MAINLOOP
            D;JGE
            @CLEAR
            0;JMP   // Done with FILL_LOOP if D>=0.

