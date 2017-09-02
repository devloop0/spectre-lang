import <"std/syscall">

func int foo(int x, int y, int i, int j) {
	double tmp1 = ((((x))) as double / y);
	double tmp2 = ((((i))) as double / j);
	return (tmp1 < tmp2) as int;
}

func int main() {
	if(foo(2, 24, 3, 4) as int == 0) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
