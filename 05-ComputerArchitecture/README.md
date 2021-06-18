# Chapter 5: Computer Architecture

The objective for this project was to build the Hack computer platform, with the topmost chip being the _Computer chip_. This chip consists of the _Memory_, _CPU_, and _ROM_:

## ROM

This chip holds the instructions for a _Hack_ program. It has 32K addressable 16-bit registers. The only input is the a 15-bit bus of an instruction address, and its output is the instruction stored at that address. It cannot be read. The ROM isn't built here since it is based on the RAM chips built in project 3.

## Memory

The memory chip's only input is the address to be probed, coming straight from the CPU's _program counter_ register. The chip consists of data memory chip (16K _RAM_), a _Screen_ chip, and a _Keyboard_ chip. The latter two are provided as built-ins, and their exposed interface is similar to RAM. Specifically, these chips interact with the computer chip through the use of memory-mapped buffers. In other words, they're each assigned a segment of memory that can be read from or written to.

### RAM

The data-memory (RAM16K) is assigned addresses 0 through 16383. Each address corresponds to a register tha can be read from or written to during the execution of a Hack program.

### Screen

The screen is assigned addresses 16384 through 24575. Writing to a bit in the screen's memory segment has the effect of coloring its associated pixel (0 for white, 1 for black).

### Keyboard

The keyboard's memory-map is just address 24576. Its value can be probed, with a 0 indicating that a key has not been pressed. If a key has been pressed, the output will be the key's ASCII value.

## CPU

The CPU chip is responsible for driving the _fetch-decode-execute_ cycle. It consists of an internal data register (_D_), a register for both data an addressing (_A_), a program counter, Hack's ALU, as well as other combinational chips that support the control logic. In summary, it works like this:

1. A fetched instruction is decoded by first deciding if it's an _A_-instruction (addressing) or a _C_-instruction (compute). In the case of the latter, the control bits are parsed.

2. Once the type of instruction has been decided, the control bits are passed to the ALU and other chips to determine what operation to perform.

3. The address of the next instruction is gotten probing the ROM register whose address is stored in the CPU's program counter. Usually, the program counter will just advance to the next instruction, but if a compute instruction issues a jump directive, the program counter is updated to reflect that.
