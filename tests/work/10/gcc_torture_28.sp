import <"std/syscall">

func int f(int y) {
	switch(y) {
		case 1: return 1;
	}
	return 0;
}

func int main() {
	if(f(1 as int) != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
