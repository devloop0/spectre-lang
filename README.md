# The Spectre Programming Language

Spectre is a C-like systems programming language that attempts to bring some modern features to C. When I read this myself, I know this sounds pretty broad, but this is honestly the best way I can describe what this project is. As of now, this programming language supports:

* Structs, functions, and all of C's control structures
* Namespaces and using statements (similar to C++)
* Digit separators (similar to C++)
* Inline assembly

A big difference from C is that pre- and post- increment semantics are a little different, and there is no short-circuit evaluation of boolean expressions.

Essentially, if you like C++ without its complex class hierarchy, Spectre may be a language for you to check out. Currently, it compiles to MIPS and I've only tested on the [MARS Simulator]( http://courses.missouristate.edu/KenVollmar/mars/) and the SPIM simulator (`sudo apt-get install spim` on Ubuntu based systems). The generated code seems to work most of the time, but on SPIM, there is a weird bug where you need to manually add a decimal point after integral `.float` directives or else its assembler will give you a syntax error. So, if you want to run the generated assembly without any modifications, just use MARS.

### Building the compiler

With all of the basic stuff out of the way, here is how to build the compiler:

1. Make sure to have GNU make installed on your system.
2. Go to the root directory of the project and run `mkdir intermediates outputs`.
3. Run `make all` (or `make release` if you want an optimized version of the compiler).
4. That's it! (The resulting binary should be in the `outputs/` folder, with the object files created by the compiler in the `intermediates/` folder).

Some other points to note:

1. This project needs C++11 to work (and the filesystem header [now part of a C++17 TS]). The compiler I've included in the `Makefile` sets the compiler as g++-6. This is the lowest version I've tested it on where it stably works. It may work on g++-5 as well, and I know it does not work on g++-4.8 (the `experimental/filesystem` header does not exist in g++-4.8).
2. As of clang++-3.5, this project fails to build (some bug in their tuple header), but I'm sure it has been fixed by now (clang++ is now on 4.0).

### A sample program

Now that the boring stuff is out of the way, here is the classic "Hello World!" program, Spectre style.

```
namespace my { # After getting import statements to work, print_string will be part of the standard library
func bool print_string(const char* str __rfuncarg__) {
    __asm__ (
        "li $v0, 4" : # The Mars and SPIM syscall for printing a string
            "syscall"
        ); # This should be it, since the __rfuncarg__ annotation for function arguments automatically places the address of the const char* str into $4 ($a0).
	return true;
    }
}

func int main() {
using my::print_string; # Places the print_string symbol from the my namespace in the current scope.
    print_string("Hello World!");
    return 0;
}
```

Now, all that's left is to compile this. Assuming you have saved this file as `temp.sp` in the root directory of the repository, and that you have aliased `java -jar Mars_4_5.jar` as `mars`, all that is required to run this program is: `outputs/spectre temp.sp` to generated the MIPS assembly file for this program, and `mars nc temp.s` to run it.

### Current limitations

The two major bugs as of now are:

1. Using `__rfuncarg__` function arguments as ternary expression lvalues will fail.
2. Register spilling that occurs in either branch of a ternary expression will fail.

### Future work

Future work will include:

1. Getting import statements working. (Copy-paste inclusion like C's `#include` preprocessor directive is also supported via the `include` statement in Spectre).
2. Getting a basic standard library working for the language (depends on \#1).


Feel free to report any bugs that you see!


