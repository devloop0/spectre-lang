import <"std/syscall">

func byte* f() {
	return f as byte*;
}

func byte* g() {}

func int main() {
	if(f() as fn byte*() != f as fn byte*() ||
	f() as fn byte*() == g as fn byte*()) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
