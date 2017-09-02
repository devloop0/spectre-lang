import <"std/syscall">
import <"std/string">

struct s { int i, j; }

func void test(type s ss) { ss.i += ss.j, ss.j -= 3; }

func void mod_test(type s* ss) { ss->i += ss->j, ss->j -= 3; }

func int main() {
	type s ss; ss.i = 4, ss.j = 5;
	char* c = stk char(5);
	c[1] = ' ', c[3] = '\n', c[4] = 0;
	c[0] = ss.i + '0', c[2] = ss.j + '0';
	std::syscall::direct_write(1, c, std::string::strlen(c));
	test(ss);
	c[0] = ss.i + '0', c[2] = ss.j + '0';
	std::syscall::direct_write(1, c, std::string::strlen(c));
	mod_test(ss$);
	c[0] = ss.i + '0', c[2] = ss.j + '0';
	std::syscall::direct_write(1, c, std::string::strlen(c));
}
