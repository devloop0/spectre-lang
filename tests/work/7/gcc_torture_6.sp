import <"std/syscall">

func int main() {
	int i = 1, j = 0;
	while(i != 1024 || j <= 0) {
		i *= 2, ++j; 
	}
	if(j != 10) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
