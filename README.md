# Spectre: A C-like compiled language.

## Why?
To sum it up simply: I like C and did it for fun. I also like namespaces.

If you enjoy programming in C, you'll definitely feel quite at home with this language.

## Contributing
* There'll probably always be more things to add to the standard library and now, more optimizations to add to the compiler (this is putting aside language features as well).
Feel free to lend a hand! Any help would always be appreciated :^).
* See the [LICENSE](LICENSE.md) for what applies to this code (and code you would contribute).
* If something went terribly wrong, feel free to let me know and I'll try my best to rectify it.
* Most importantly, this project was a chance for me to build something from the ground up and learn how stuff I works. If you are reading this and want to contribute,
I'd like to keep this spirit going and hope you feel the same way.

## License
See the [LICENSE](LICENSE.md) file.

## Dependencies
* CMake (>= 3.0)
* g++\-9 (C++17)
* realpath
* GNU binutils

## Compiler Setup
To compile spectre for your machine:
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_CXX_COMPILER=g++-9 ..
$ make
```
This will setup and compile spectre for you.

## Library Setup
The standard library only works on an actual 32-bit ARMV6L Linux system. If you don't have access to one, you'll need to emulate one.
First, you'll need to copy the `libspectre` directory into your `/usr/include` directory. The spectre compiler searches for system includes from the
`/usr/include/libspectre` path. I'd recommend giving your current user full permissions over the folder during setup (you can change this later if you want).
Additionally, you'll need to add the spectre compiler to your `$PATH` for this build script to work.
From here on out, I'll assume that you have full permissions to the `/usr/include/libspectre` folder and `spectre` is in your `$PATH`.
Now, do the following (inside the `/usr/include/libspectre` folder for example):
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
Running assumes you've compiled the standard library from source in the last step (see the `Config` section for more clarifications on this) and you're on a 32-bit Linux ARMV6L system.
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
You could also try the more ergonomic printf function as well:
```
import <"std/io">

using std::io::printf;

func int main() {
	printf("Hello World!\n");
	return 0;
}
```

Save either snippet in a file called `hello.sp` (or really whatever you want, I'll just assume `hello.sp` from here on out).
Now, run the following:
```
$ spectre hello.sp
$ as -mfloat-abi=hard -mfpu=vfp hello.s -o hello.o
$ ld hello.o -o hello -L/usr/include/libspectre -l:libspectre.a
$ ./hello
```
This should print `Hello World!` if everything went smoothly. Since these sets of commands are so common, there's a shortcut for doing this:
```
$ sp hello.sp
$ ./hello
```
Where the `sp` command is just a bash script wrapper around the `spectre` compiler.

## Tips
If you don't have access to a physical ARMV6L machine, I would recommend using QEMU to emulate one. I've tested this on a QEMU 32-bit emulated CPU that has floating point support
with a Raspbian Linux distribution.
If you are looking for the old, MIPS backend, check out [this README](MIPS_AST_backend_README.md).

## Testing
Inside the `tests/` folder, there are two folders, `work/` and `ref_outputs/`. `work/` contains tests you can automatically run (see below). `ref_outputs/` contains the
reference outputs for the tests.
Once again, this assumes the standard library from before.
```
$ ./clear_outputs.sh
$ ./check_outputs
$ ./run.sh
```
This will run all the tests in each directory, pipe outputs and return values to files, and check the outputs of the tests to see if they match the reference outputs.
These folders contain examples of practical spectre programs such as linked lists, ternary search trees, etc. So if you're confused about how to use spectre, this is a good place to start!

## Samples
Inside the `samples/` folder, you will see some simple spectre programs that display the power of the spectre programming language and standard library.
In addition to the tests, these samples are a great place to start seeing spectre in action in a practical setting.
These currently include things like a decimal to hexadecimal converter (and vice versa), a simple echo program, and more. 
The more complex ones include:
* A full expression evaluator. All of C's binary operators are supported; multiple statements (separated by semicolons) are also supported as well.
This is in the `samples/expr_eval/` folder.
* A simple ls clone; this includes some of the output provided by the traditional `ls -la` including displaying hidden files, file permissions, file sizes, etc.
* A simple du clone; this is essentially the output of `du -b` on regular files.

## Future Work
* Should make the `sp` script more robust by just doing the entire thing in C++ so it'll handle more options as well. Currently, it takes all files as inputs and assumes your first file is the file with your main function.
* ~~Should make testing more robust and intuitive (this includes updating all tests to include the `test.sp` include and automate checking for tests).~~ This is being accomplished with the `tests/work/check_outputs.sh` script.
See the `Testing` section above.
* Implement more optimizations (copy propagation, CSE, etc.).
* Implement pre-coloring and support register coalescing in `alloc_regs.{c,h}pp`. Also make the algorithm more efficient.
* Look into linking calls dynamically; currently everything is statically linked which makes life easier, but dynamic linking should be supported.

