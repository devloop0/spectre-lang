import <"std/syscall">

func float f(float f1, int x2, double d3, float f4, int x5, double d6,
	float f7, int x8, double d9) {
	return f1 * x2 * d3 + f4 * x5 * d6 + f7 * x8 * d9;
}

func int main() {
	float r = f(2, 3, 4, 5, 6, 7, 8, 9, 10);
	if(r == 954.)
		std::syscall::direct_write(1, "Y\n", 2);
	else
		std::syscall::direct_write(1, "N\n", 2);
}
