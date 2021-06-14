# Chapter 3: Sequential Logic

The goal of this project was to build a hierarchy of memory units as well as a program counter. All of these chips are _sequential_ (i.e., _clocked_), which means their behavior is time-dependent. In particular, they're all based on a _data flip-fop (DFF)_ gate, which was treat as atomic just like the _Nand_ gate for combinational gates. The significance of the DFF is that it simply outputs its input at the previous time-step, giving us a way to implement storage.

1. _Bit_: Simplest storage unit which stores 1 _bit_ (a single 0 or 1).

2. _Register_: An array of 16 Bits, or a 16-bit _word_.

3. _RAM8_: A chip that contains 8 registers, with addressing logic to choose the correct register to probe from or to load new data into.

4. _RAM64, RAM512, RAM4K, RAM16K_: Similar to RAM8, but each is based on the previous, so RAM64 is based on RAM8, RAM512 is based on RAM64, and so on.

5. _PC_: an implementation of a program instruction counter. Generally it will go to the next instruction (add 1 to its own value). But at times it will be reset to 0 (presumably the first instruction) or set to _d_ (jump to instruction with address _d_).

Together with the chips built in previous chapters, the sequential chips allow us to build the CPU, ROM, and other top-level components of the Hack system.
