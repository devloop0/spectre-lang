import <"std/syscall">
import <"std/string">

using std::syscall::direct_write;
using std::syscall::exit;
using std::string::strcmp;

func int bar(const char** x1) {
	char** x = x1 as char**;
	char tmp = x@@;
	return x@ = x@[1]$, tmp;
}

func int baz(int c) {
	return (c != '@') as int;
}

func void foo(const char** w1, char* x, bool y, bool z) {
	char** w = w1 as char**;
	char c = bar(w);
	int i = 0;
	while(1 as bool) {
		x[i++] = c;
		c = bar(w);
		if(y && c == '\'') break;
		if(z && c == '\"') break;
		if(!y as bool && !z as bool && !baz(c) as bool) break;
	}
	x[i] = 0;
}

func int main() {
	char* buf = stk char(64);
	char* p;
	p = "abcde'fgh";
	foo(p$, buf, 1 as bool, 0 as bool);
	if(strcmp(p, "fgh") != 0 || strcmp(buf, "abcde") != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	p = "ABCDEFG\"HI";
	foo(p$, buf, 0 as bool, 1 as bool);
	if(strcmp(p, "HI") != 0 || strcmp(buf, "ABCDEFG") != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	p = "abcd\"e'fgh";
	foo(p$, buf, 1 as bool, 1 as bool);
	if(strcmp(p, "e'fgh") != 0 || strcmp(buf, "abcd") != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	p = "ABCDEF'G\"HI";
	foo(p$, buf, 1 as bool, 1 as bool);
	if(strcmp(p, "G\"HI") != 0 || strcmp(buf, "ABCDEF") != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	p = "abcdef@gh";
	foo(p$, buf, 0 as bool, 0 as bool);
	if(strcmp(p, "gh") != 0 || strcmp(buf, "abcdef") != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
