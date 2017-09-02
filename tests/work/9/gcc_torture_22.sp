import <"std/syscall">

func int f(unsigned int bitcount, int mant) {
	int mask = -1 << bitcount;
	int retval = -1;
	if(!(mant & -mask) as bool)
		retval = 0;
	if((mant & ~mask) as bool)
		retval = 1;
	if(retval == -1)
		retval = 0;
	return retval;
}

func int main() {
	if(f(0, -1) as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
