import <"std/syscall">

func int foo(int a) {
	int x = 0 % a++;
	return a;
}

func int main() {
	if(foo(9) != 10) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
