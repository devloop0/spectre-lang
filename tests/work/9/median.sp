import <"std/syscall">

func int median(int a, int b, int c) {
	return a < b ? b < c ? b : a < c ? c : a : a < c ? a : b < c ? c : b;
}

func int main() {
	int x = 1, y = 2, z = 3;
	if(median(1, 2, 3) != 2 || median(2, 1, 3) != 2 || median(3, 1, 2) != 2
		|| median(2, 3, 1) != 2 || median(3, 2, 1) != 2
		|| median(1, 3, 2) != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
