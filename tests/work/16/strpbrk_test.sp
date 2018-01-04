import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	const char* str1 = "abcde3kjs253h;", str2 = "35";
	char* ret1 = strpbrk(str1, str2), ret2 = strpbrk(ret1[1]$, str2);

	if(strcmp(ret1, "3kjs253h;") != 0 || strcmp(ret2, "53h;") != 0) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else {
		direct_write(1, "Y\n", 2);
		return 0;
	}
}
