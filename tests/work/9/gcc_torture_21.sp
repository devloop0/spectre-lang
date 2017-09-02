import <"std/syscall">

static int x;

func int bar() {
	return x;
}

func int foo() {
	long b = bar();
	if(b as unsigned long < -4095_l)
		return b;
	if(-b != 38)
		b = -2;
	return b + 1;
}

func int main() {
	x = 26;
	if(foo() != 26) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	x = -39;
	if(foo() != -1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	x = -38;
	if(foo() != -37) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
