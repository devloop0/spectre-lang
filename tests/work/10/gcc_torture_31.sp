import <"std/syscall">

func int f(unsigned char x) {
	return (0x50 | (x >> 4)) ^ 0xff;
}

func int main() {
	if(f(0) != 0xaf) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
