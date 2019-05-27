# Spectre: A C-like compiled language.

## Why?
I like C and did it for fun. I also like namespaces.

## Dependencies
* make
* g++\-9 (C++17)
* realpath
* GNU binutils

## Compiler Setup
To compile spectre for your machine:
```
$ make install
```
This will setup and compile spectre for you.

## Library Setup
The standard library only works on an actual 32-bit MIPS Linux system. If you don't have access to one, you'll need to emulate one.
First, you'll need to copy the `libspectre` directory into your `/usr/include` directory. The spectre compiler searches for system includes from the
`/usr/include/libspectre` path. I'd recommend giving your current user full permissions over the folder during setup (you can change this later if you want).
Additionally, you'll need to add the spectre compiler to your `$PATH` for this build script to work.
From here on out, I'll assume that you have full permissions to the `/usr/include/libspectre` folder and `spectre` is in your `$PATH`.
Now, do the following:
```
$ mkdir build
$ cd build
$ /usr/include/libspectre/build.sh
$ cp libspectre.a /usr/include/libspectre/
```
This creates a temporary build directory and builds the standard library and statically links it to a `libspectre.a` archive.
Then you'll need to place this in the `/usr/include/libspectre` folder since this is spectre's system folder.
You can remove the `build/` folder now if you want.

## Running
Running assumes you've compiled the standard library from source in the last step (see the `Config` section for more clarifications on this) and you're on a 32-bit Linux MIPS system.
Here's a sample Spectre program to print `Hello World!` using syscalls.
```
import <"std/syscall">
import <"std/string">

using std::syscall::direct_write;
using std::string::strlen;

func int main() {
	char* hello = "Hello World!\n";
	direct_write(1, hello, strlen(hello));
	return 0;
}
```
Save this in a file called `hello.sp` (or really whatever you want, I'll just assume `hello.sp` from here on out).
Now, run the following:
```
$ spectre hello.sp
$ as -mips32 hello.s -o hello.o
$ ld hello.o -o hello -L/usr/include/libspectre -l:libspectre.a
$ ./hello
```
This should print `Hello World!` if everything went correct. Since these sets of command are so common, there's a shortcut for doing this:
```
$ sp hello.sp
$ ./hello
```
Where the `sp` command is just a bash script wrapper around the `spectre` compiler.

## Tips
If you don't have access to a physical MIPS machine, I would recommend using QEMU to emulate MIPS. I've tested this on a QEMU 32-bit MIPS Malta emulated CPU that has floating point support.

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

The `BACKEND` can be toggled independently of the `SYSTEM` macro. Note that as of now `BACKEND_IR` is the default backend, so that means Spectre can/might not generate assembly at all.
If you would like to go back to the old `BACKEND`, switch the `BACKEND` config flag to `BACKEND_AST` instead of `BACKEND_IR`.


## Testing
Inside the `tests/` folder, there are two folders, `work/` and `work_heavy/`. Both contain tests. You'll need to compile `work_heavy` manually (these take a long time to compile) but you can automatically run everything in the `work/` folder.
Once again, this assumes the standard library from before.
NOTE: the following output clears the reference outputs in each folder called `test_outputs` if you want to compare to make sure things work, you'll need to save these somewhere else.
```
$ ./clear_outputs.sh
$ ./run.sh
```
This will run all the tests in each directory and pipe outputs and return values to files. These folders contain examples of practical spectre programs such as linked lists, ternary search trees, etc. So if you're confused about how to use spectre, this is a good place to start.

## Samples
Inside the `samples/` folder, you will see some simple spectre programs that display the power of the spectre programming language and standard library. In addition to the tests, these samples are a great place to start seeing spectre in action in a practical setting. These currently include things like a decimal to hexadecimal converter (and vice versa), a simple echo program, and more. 

## Future Work
* Should make the `sp` script more robust by just doing the entire thing in C++ so it'll handle more options as well. Currently, it takes all files as inputs and assumes your first file is the file with your main function.
* Should make testing more robust and intuitive (this includes updating all tests to include the `test.sp` include and automate checking for tests).
* Write a tutorial? (new now defaults to a malloc call, which should be more intuitive).
* Consider memcpy integration into the compiler: would force the compiler to no longer support non-MIPS system targets, but would clean up code for copying strings and structs.
* There's an edge-case bug where parenthesized rvalues that get converted into lvalues (i.e. `int i = 2; byte* b = i$ as int*; (b as int*)@ *= 2`), will fail. However, `b as int* @ *= 2` and `(b as int* @) *= 2` will both work. This can probably be mitigated by re-writing the back-end by first converting to a LIR and then to machine code intelligently rather than the current model of traversing the high-level AST and converting that into machine code. In the mean time, hopefully this bug isn't too critical.

## Contributions
* If something went horribly wrong, let me know and I'll try to fix it. Of course, if you want to fix it yourself, contributions are always welcome :^)

## License
* If you modify the compiler or standard library, make it open source if you plan on re-distributing it.
* If you write something in the spectre programming language, link against or just use the spectre standard library, or build an executable using spectre, do whatever you want.
