# The Spectre Programming Language

Spectre is a C-like systems programming language that attempts to bring some modern features to C. When I read this myself, I know this sounds pretty broad, but this is honestly the best way I can describe what this project is. As of now, this programming language supports:

* Structs, functions, and all of C's control structures
* Namespaces and using statements (similar to C++)
* Digit separators (similar to C++)
* Inline assembly
* And more!...

A big difference from C is that pre- and post- increment semantics are a little different, and there is no short-circuit evaluation of boolean expressions.

Essentially, if you like C++ without its complex class hierarchy, Spectre may be a language for you to check out. Currently, it compiles to MIPS and I've only tested on the [MARS Simulator]( http://courses.missouristate.edu/KenVollmar/mars/) and the SPIM simulator (`sudo apt-get install spim` on Ubuntu based systems). The generated code seems to work most of the time, but on SPIM, there is a weird bug where you need to manually add a decimal point after integral `.float` directives or else its assembler will give you a syntax error. So, if you want to run the generated assembly without any modifications, just use MARS.

### Building the compiler

With all of the basic stuff out of the way, here is how to build the compiler:

1. Make sure to have GNU make installed on your system.
<s>
2. Go to the root directory of the project and run `mkdir intermediates outputs`.
3. Run `make all` (or `make release` if you want an optimized version of the compiler).
4. That's it! (The resulting binary should be in the `outputs/` folder, with the object files created by the compiler in the `intermediates/` folder).
</s>

Now it's even simpler:
2. Run `make install`. This will create the necessary directories and run `make all`. The binaries will be in the `outputs` folder and the the object files will be in the `intermediates` folder.

### Building the standard library

There is also a Makefile to build the standard library as well, although to get it to work, you will need to add some variables to your `~/.bashrc` file and symlinks to get spectre working on your computer. Please make sure to build the compiler first before doing this step, as building the standard library for spectre requires the compiler itself to be working first. 

Some assumptions: 1) You have installed spectre on a working Linux distribution; the Makefile provided only works on Linux, so if you followed the previous step, you are probably on one anyways. 2) You have superuser privileges; this is so you can symlink your the standard library into the system include directory. 3) I am going to assume that the root of the github repo you cloned is in `~/spectre`; if it is not, you will need to modify the following instructions appropriately. Now that all of this is out of the way, let's get started!

1. Before anything, you want to make sure that the compiler will be able to find the standard library that is provided with this repo. To do this, we are going to symlink (a soft link) the `spectre-include` directory to a `spectre` directory inside `/usr/include`. First, `cd` into tnto `/usr/include` like so: `cd /usr/include`; then symlink the `~/spectre/spectre-include` folder with a `spectre` folder inside `/usr/include` like so: `sudo ln -s ~/spectre/spectre-include spectre`. This is the only part where superuser privileges are required. If you `ls` here, you will see a new folder called `spectre` which you can `cd` into; if you also go inside the `std` folder, you will see the current implementation of the standard library. The entire implementation is inside the `spectre` folder so as to prevent clobbering our files with any system files that you have on your computer. If you ever want to remove this folder, just run `sudo rm spectre` from this directory and the symlink will be removed. 
2. Now it's time to add some environment variables. Open `your ~/.bashrc` in your favorite text editor.
3. Go to the end of the file and type these following lines:

```bash
alias sp="$HOME/spectre/outputs/spectre"
export SP_BIN=$HOME/spectre/spectre-outputs
export SP_INCLUDE=$HOME/spectre/spectre-include
export SP=$HOME/spectre
```

Here's what these all do: the first aliases the spectre executable to `sp`; this is simply for convenience so later, when you actually write spectre code, you can just compile using `sp` instead of `$HOME/spectre/outputs/spectre`. The second, third, and fourth lines add names for directories that will be useful later; although the spectre Makefile itself doesn't rely on these aliases, it will make your life a lot easier, especially if you decide to contribute to this repo.

Now just `source ~/.bashrc` and you should be good to go!

4. Now, we are ready to run the compiler; go back to the folder where you cloned the repo and built the compiler and just run `make -f sp_Makefile install`. This will do several things; it will look inside the `spectre-include` folder provided with the repo and compile all of the standard library implementation files and place the resulting assmeblies inside a newly created folder called `spectre-outputs` (aliased to `$SP_BIN`). If you modify something in the standard library, just run `make -f sp_Makefile all` to recompile the standard library. To remove the outputs folder, just run `make -f sp_Makefile uninstall`. The `sp_Makefile` does not mess with your system at all, so feel free to `install` and `uninstall` as many times as you like. (This also applies with the compiler Makefile as well). In fact, the only change to your system that you should have made were the lines added to your `~/.bashrc` and the symlink created in `/usr/include`, both of which are easily reversible.

You are good to go! Look at the sample program section to see how to use spectre.

Some other points to note:

1. This project needs C++11 to work (and the filesystem header [now part of a C++17 TS]). The compiler I've included in the `Makefile` sets the compiler as g++-6. This is the lowest version I've tested it on where it stably works. It may work on g++-5 as well, and I know it does not work on g++-4.8 (the `experimental/filesystem` header does not exist in g++-4.8).
2. As of clang++-3.5, this project fails to build (some bug in their tuple header), but I'm sure it has been fixed by now (clang++ is now on 4.0).

### A sample program

Now that the boring stuff is out of the way, here is the classic "Hello World!" program, Spectre style.
If you have not followed the instructions for building the compiler and building the standard library, be sure to do so, since this sections assumes you have done that.

<s>
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
</s>

The above was the old way of doing things; let's look at the updated version using the standard library.

```
import <"std/io">
# Imports the io module from the standard library.

func int main() {
using std::io::print_string_ln; # Print a string to standard output.
    print_string_ln("Hello World!"); # Print "Hello World!" to the command line; this will automatically insert a new line at the end as well.
    return 0; # No errors occurred.
}
```

That's it! No need to manually handle assembly (this is done for you by the standard library). To run this, save this code into a file called `temp.sp`. Inside your terminal, run `sp temp.sp`; there should be no warnings or errors (if there were, you probably installed the standard library incorrectly, redo the previous section and be sure to follow everything precisely). There should be an assembly file called `temp.s` in your current directory with a bunch of MIPS code. You don't need to worry about this; this is just the assembly code that the compiler generated for your source file. 

Here, I am going to assume that you have aliased your Mars simulator to `mars` (i.e. `java -jar Mars_4_5.jar`). Now, all that you need to do is the following: `mars nc $SP_BIN/* temp.s` and the string `Hello World!` should appear on your standard output followed by a new line. Be sure to place `$SP_BIN/*` before `temp.s`, since everything inside the standard library needs to be initialized first before your program is run.

If you have gotten this far, congratulations! Spectre now works on your system, so feel free to mess around with this as much as you would like to. Some documentation for the language will be coming soon, but the language is somewhat similar to C, so if you are familiar with C, you should be able to pick up Spectre extremely quickly.

### Current limitations

The three major bugs as of now are:

1. Using `__rfuncarg__` function arguments as ternary expression lvalues will fail.
2. Register spilling that occurs in either branch of a ternary expression will fail.
3. Using `__rfuncarg__` function arguments as struct's through multiple `__rfuncarg__` functions results in errors.

### Future work

Future work will include:

1. ~~Getting import statements working. (Copy-paste inclusion like C's `#include` preprocessor directive is also supported via the `include` statement in Spectre).~~ Done!
2. Getting a basic standard library working for the language (depends on \#1). -- Work in progress.
3. Get some documentation on the language.


Feel free to report any bugs that you see!



