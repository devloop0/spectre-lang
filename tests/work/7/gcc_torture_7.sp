import <"std/syscall">

func unsigned int foo(unsigned int x) {
	return x = x < 5 ? 4 : 8;
}

func int main() {
	if(foo(8) != 8) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
