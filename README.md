Spectre: A C-like compiled language.

Sample:

```
namespace std {
	namespace io {
		func void print_string(const char* str) {
			__asm__ ( "li $2, 4" : str "$4" : "lw $4, 0($4)" : "syscall" );
		}
	}
}

namespace std {
	namespace machine {
		func const bool little_endian() {
			int x = 1;
			return x$ as char* @ as bool;
		}
	}
}

func int main() {
	std::io::print_string("Hello World!\n");
	std::io::print_string(std::machine::little_endian() ? "This is a little endian machine.\n" : "This is a big endian machine.\n");
}
```
