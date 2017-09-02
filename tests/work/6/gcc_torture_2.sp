import <"std/syscall">

func unsigned short showbug(unsigned short* a, unsigned short* b) {
	a@ += b@ - 8;
	return (a@ >= 8) as unsigned short;
}

func int main() {
	unsigned short x = 0, y = 10;
	if(showbug(x$, y$) != 0) std::syscall::direct_write(1, "N\n", 2);
	else std::syscall::direct_write(1, "Y\n", 2);
}
