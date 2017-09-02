import <"std/syscall">

func int main() {
	double x = 1., y = 2.;
	if((y > x--) as int != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
