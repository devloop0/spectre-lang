import <"std/syscall">

func void putchar(const char c) {
	std::syscall::direct_write(1, c$, 1);
}

int c = 1;

func int main() {
	putchar(c+'@'); (c^'?'^'%') as bool?(++c,main()):c;
}
