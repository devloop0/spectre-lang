import <"std/syscall">

func void f(int* x) {
	x@ = 0;
}

func int main() {
	int s, c, x;
	char* a = stk char(2); a[0] = 'c', a[1] = 0;
	f(s$);
	a[c = 0] = s == 0 ? (x = 1, 'a') : (x=2, 'b');
	if(c != 0 || a[c] != 'a' || x != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
