import <"std/syscall">

func int main() {
	int a = 0;
	if((
		(a == 3) ? 3 : ((((a = 3)))) as bool ? 5 : 0
	) != 5 || a != 3) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	int x = 0, y = 0;
	(((((((((x = 2)))) == ((((y = 3)))) ? x = 4 : ((((y = 5))))))))) = 10;
	if(x != 2 || y != 10) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
