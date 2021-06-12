# Chapter 2: Boolean Arithmetic

Continuing from the basic gates created in project 1, this project's goal was to create Hack's ALU. To represent signed numbers, a _2's complement_ method was chosen. A variety of gates were developed based on this scheme:

1. _HalfAdder_: Computes the sum of two bits `a` and `b` and outputs the `carry` (MSB of `a+b`) bit and the `sum` bit (MSB of `a+b`).

2. _FullAdder_: Computes the sum of three bits `a`, `b`, and `c`. Outputs a `carry` and `sum` bit, as defined in (1).

3. _Adder_: Computes the sum of two 16-bit buses `x` and `y`, and outputs the `sum`, disregarding overflow.

4. _Incrementer_: Add `1` to the input `x`, disregarding overflow.

5. _ALU_: Takes as input two 16-bit buses `x` and `y`, as well as 6 control bits, and operates on the buses depending on the control bits (there are 64 possible functions).

Though the _Incrementer_ chip was implemented here, it was not used to implement the ALU chip; it will be used in future projects.
