import <"std/syscall">

struct s {
	int my_int;
	fn void(type s*, int) set_my_int;
}

func void set_my_int(type s* this, int i) {
	this->my_int = i;
}

func int main() {
	char* c = stk char(2);
	c[1] = '\n';
	type s ss; ss.my_int = 5, ss.set_my_int = set_my_int;
	c@ = ss.my_int + '0';
	std::syscall::direct_write(1, c, 2);
	ss.set_my_int(ss$, 8);
	c@ = ss.my_int + '0';
	std::syscall::direct_write(1, c, 2);
	return 0;
}
