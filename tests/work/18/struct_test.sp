import <"std/string">

import "../test.sp"

struct s { int i; char* d; int j; char* c; }

func type s f(type s ss) {
	ss.i = 945'123;
	ss.j = 756'232;
	std::string::strcat(ss.c, " World");
	std::string::strcat(ss.d, "Goodbye");
	return ss;
}

func int main() {
	type s ss;
	ss.i = 123'456;
	ss.j = 789'101;
	ss.c = stk char(100);
	ss.d = stk char(100);
	std::string::strcpy(ss.c, "Hello");
	std::string::strcpy(ss.d, "World, ");
	if(ss.i != 123'456 || ss.j != 789'101 ||
		std::string::strcmp(ss.c, "Hello") != 0 ||
		std::string::strcmp(ss.d, "World, ") != 0)
		fail();
	ss = f(ss);
	if(ss.i != 945'123 || ss.j != 756'232 ||
		std::string::strcmp(ss.c, "Hello World") != 0 ||
		std::string::strcmp(ss.d, "World, Goodbye") != 0)
		fail();
	success();
}
