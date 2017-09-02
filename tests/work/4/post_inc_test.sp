import <"std/syscall">

func int main() {
	using std::syscall::direct_write;
	char* c = stk char(2); c[1] = '\n';
	int x = 2;
	int* p = x$;
	c@ = p@ + '0';
	direct_write(1, c, 2);
	((((((p)))@)))++;
	c@ = x + '0';
	direct_write(1, c, 2);
	return 0;
}
