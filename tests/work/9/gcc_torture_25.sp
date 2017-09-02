import <"std/syscall">

func int ffs(int x) {
	int bit, mask;
	if(x == 0) return 0;
	bit = 1, mask = 1;
	for(; !(x & mask) as bool; bit++, mask <<= 1);
	return bit;
}

func int f(int x) {
	int y;
	y = ffs(x) - 1;
	if(y < 0) {
		std::syscall::direct_write(1, "N\n", 2);
		std::syscall::exit(1);
	}
}

func int main() {
	f(1);
	std::syscall::direct_write(1, "Y\n", 2);
}
