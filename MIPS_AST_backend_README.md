# Old Backend
The new backend that'll officially be worked on from now on involves an IR that can be optimized and targeted towards numerous assembly language platforms.
Currently, the only platform supported is ARMV6L; since MIPS hasn't been ported, I have left that backend there. It should be relatively stable, since it passes
a core set of tests in `tests/`. However, there'll probably be some minor issues here and there due to the fact it goes from AST -> assembly directly while
also avoiding using a stack machine implementation. As a result, there might be some register coloring problems or inefficiencies. Note that this backend is not
currently optimizing.

The following sections describe toggling configurations for this backend (including how to turn it on, since it is off by default), and future work that could done on
this backend (this is separate from the current AST -> IR -> assembly backend discussed in length in the other README).

## Toggling the Backends
I've hidden the old MIPS backend behind a toggle-able `BACKEND` macro. Note that `BACKEND_IR` is the one that is currently being worked on and is the default
backend. This backend currently only supports targetting ARMV6L.
If you would like to go back to the old `BACKEND`, switch the `BACKEND` config flag to `BACKEND_AST` instead of `BACKEND_IR`. This will take you back to the
MIPS backend and everything beneath here applies then. None of this applies to `BACKEND_IR` (this is the one that is currently on by default).

## Future Work
* Write a tutorial? (new now defaults to a malloc call, which should be more intuitive).
* Consider memcpy integration into the compiler: would force the compiler to no longer support non-MIPS system targets, but would clean up code for copying strings and structs.
* There's an edge-case bug where parenthesized rvalues that get converted into lvalues (i.e. `int i = 2; byte* b = i$ as int*; (b as int*)@ *= 2`), will fail. However, `b as int* @ *= 2` and `(b as int* @) *= 2` will both work. This can probably be mitigated by re-writing the back-end by first converting to a LIR and then to machine code intelligently rather than the current model of traversing the high-level AST and converting that into machine code. In the mean time, hopefully this bug isn't too critical.

## Config 

If you absolutely don't have access to a physical MIPS system, you'll lose access to the standard library, but there are still ways to use spectre.
Inside the `config.hpp` file, you'll see some preprocessor macros that you can change to change the behavior of spectre itself.
Please only touch the `SYSTEM`, `PROG_TERM`, `PROG_NEW`, and `BACKEND` macros.
Here are the options for the `SYSTEM` macro:
* `REAL_MIPS`: the default, spectre compiles the program as if it were going to be run on a real Linux machine.
* `MARS`: spectre compiles the program as if it were going to be run on the MARS MIPS emulator.
* `RISCV`: spectre compiles the program as if it were going to be run on a RISCV machine.

Here are the options for the `PROG_TERM` macro:
* `PROG_TERM_ABORT`: at the end of the main function, the program terminates by essentially calling the `abort()` function in C (something similar to this). More importantly, no exit handlers are run.
* `PROG_TERM_EXIT`: the default, runs exit handlers on program termination, require integration into the standard library by the compiler.

Here are the options for the `PROG_NEW` macro:
* `PROG_NEW_SBRK`: calls the sbrk syscall. Does not require a malloc implementation.
* `PROG_NEW_MALLOC`: more intuitive, and the memory you allocate can actually be freed. However, this requires an actual mips system.

Here are the options for the `BACKEND` macro:
* `BACKEND_AST`: the (old) backend that generates MIPS assembly straight from a program's AST.
* `BACKEND_IR`: generates an IR first (with potentially some optimization passes), and will eventually generate assembly from the IR.

These options fall into the following sets:
* `REAL_MIPS` - { `PROG_TERM_ABORT`, `PROG_TERM_EXIT`, `PROG_NEW_MALLOC` }
* `MARS` - { `PROG_TERM_ABORT`, `PROG_NEW_SBRK` }
* `RISCV` - { `PROG_TERM_ABORT`, `PROG_NEW_SBRK` }

In other words, if you're compiling for MARS, you can only use `PROG_TERM_ABORT` option.

