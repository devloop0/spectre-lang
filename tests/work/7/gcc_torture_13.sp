import <"std/syscall">

func int abs(int x) {
	return x < 0 ? -x : x;
}
	
func void check(int* p) {
	int i = 0;
	for(; i < 5; i++)
		if(p[i] as bool) {
			std::syscall::direct_write(1, "N\n", 2);
			std::syscall::exit(1);
			return;
		}
	for(; i < 10; i++)
		if(p[i] != i + 1) {
			std::syscall::direct_write(1, "N\n", 2);
			std::syscall::exit(1);
			return;
		}
}

func int main() {
	int* a = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ];
	int i = -5;

	for(; i < -1; i++)
		a[(i - 10 < 0 ? 10 - i : i - 10) - 11] = 0;
	a[abs(i - 10) - 11] = 0;
	check(a);
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
