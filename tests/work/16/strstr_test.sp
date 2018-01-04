import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;
	
	const char* str1 = "NikhilsdfjnA", str2 = "sdf";
	if(strcmp(strstr(str1, str2), "sdfjnA") != 0 || strcmp(strstr("ABC", "ABC"), "ABC") != 0 ||
		strstr("AB", "ABC") != NULL) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else {
		direct_write(1, "Y\n", 2);
		return 0;
	}
	direct_write(1, "N\n", 2);
	return 1;
}
