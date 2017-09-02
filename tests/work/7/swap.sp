import <"std/syscall">

func void swap(int* x, int* y) {
	x@ ^= y@ ^= x@ ^= y@;
}

func int main() {
	int a = 2, b = 3;
	swap(a$, b$);
	if(a != 3 || b != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
