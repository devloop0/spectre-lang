import <"std/syscall">

func unsigned int foo(char* c, unsigned int x, unsigned int y) {
	unsigned int z = x + 1;
	return z / (y + 1);
}

func int main() {
	char* c = stk char(16);
	if(foo(c, ~1_ui, 4) != (~0_ui / 5)) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
