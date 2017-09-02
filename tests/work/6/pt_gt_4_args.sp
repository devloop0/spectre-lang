import <"std/syscall">

func void test(int x1, double d2, float f3, int x4, int x5, double d6,
	float f7, int x8, int* ptr, int x9) {
	ptr@ = x1 * d2 * f3 - x4 * x5 * d6 + f7 * x8 * x9;
}

func int main() {
	int res;
	test(5, 6, 7, 8, 9, 10, 11, 12, res$, 13);
	if(res != 1206) std::syscall::direct_write(1, "N\n", 2);
	else std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
