import <"std/syscall">

struct s { int i, j; }

func int test(type s ss) { ss.i += ss.j, ss.j -= 3; return ss.i + ss.j; }

func type s* mod_test(type s* ss) { ss->i += ss->j, ss->j -= 3; return ss; }

func int main() {
	type s ss; ss.i = 4, ss.j = 5;
	if(test(ss) != 11 || ss.i != 4 || ss.j != 5) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(type s temp = ss = mod_test(ss$)@; ss.i != 9 || ss.j != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	mod_test(ss$);
	if(ss.i != 11 || ss.j != -1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y" "\n", 2);
	return 0;
}
