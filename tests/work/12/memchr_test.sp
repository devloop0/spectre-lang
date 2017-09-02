import <"std/string">
import <"std/syscall">

using namespace std::string;
using namespace std::syscall;

func void fail() {
	direct_write(1, "N\n", 2);
	exit(1);
}

func int main() {
	char* pch, str = "Example string";
	pch = memchr(str as byte*, 'p', strlen(str)) as char*;
	if(pch as unsigned int - str as unsigned int + 1 != 5)
		fail();
	str = "ABCDEFG";
	char* ps = memchr(str as byte*, 'D', strlen(str)) as char*;
	if(ps as bool) {
		if(strcmp(ps, "DEFG") as bool)
			fail();
	}
	else
		fail();
	char* arr = [ 'a', 0 as char, 'a', 'A', 'a', 'a', 'A', 'a' ];
	char* pc = memchr(arr as byte*, 'A', 8) as char*;
	if(!pc as bool) fail();
	str = "http://www.google.com";
	const char ch = '.';
	char* ret;
	ret = memchr(str as byte*, ch, strlen(str)) as char*;
	if(strcmp(ret[1]$, "google.com") as bool) fail();
	pc = memchr(arr as byte*, 'B', 8) as char*;
	if(pc as bool) fail();
	direct_write(1, "Y\n", 2);
	return 0;
}
