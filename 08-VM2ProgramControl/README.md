# Chapter 8: Virtual Machine 2: Program Control

The objective of this project was to extend the VM translator built in the previous project to a full scale translator. To that end, the translator was made to handle program flow (like `goto` and `if-goto` conditional execution) and function calling VM commands.

## Context

The VM translator in the previous project was written under the consideration that the VM is based on a _stack machine model_. The same model lends itself quite nicely to support conditional logic and subroutine (function) calling.

### Program Flow

The VM language uses the `label`, `goto`, and `if-goto` commands to support conditional execution.

- `label labelName`: Marks a location that can then be jumped to using `goto` and `if-goto`.

- `goto labelName`: Unconditionally jump to the instruction address corresponding to `labelName`.

- `if-goto labelName`: Performs a jump if the value at the top of the stack is nonzero.

### Subroutine Calling

To support calling routines, the VM language uses the `function`, `call`, and `return` commands.

- `function functionName k`: Creates a label for entry point of the function named `functionName`. In addition, it initializes the `k` local variables to 0 by pushing 0s onto the stack.

* `call functionName n`: A total of `n` arguments are pushed to the stack before this command. The command pushes the address of the command following `call` so that the `functionName` can `return` to it. Moreover, `call` saves the _stack frame_ of the caller and repositions pointers to provide a clean slate for the called function.

* `return`: When called, it puts the return value in the correct place for the caller. It then repositions the stack pointer and restores the caller's stack frame before returning to the caller.

## Implementation

The VM translator that I have built is an implementation of the proposed API in the project guidelines. The program takes one command-line argument which may be a VM file or a directory containing VM files. The output is a single assembly file whose commands were translated from all VM files presented to it.
