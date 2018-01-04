import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::syscall;
	using namespace std::string;

	char* str1 = stk char(15), str2 = stk char(15),
		str3 = stk char(15);
	
	strcpy(str1, "abcdef");
	strcpy(str2, "ABCDEF");
	strcpy(str3, "abcdEF");

	int ret = strncmp(str2, str1, 4);
	int ret2 = strncmp(str1, str3, 4);
	int ret3 = strncmp(str1, str3, strlen(str1));

	if(ret != -1 || ret2 != 0 || ret3 != 1) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else {
		direct_write(1, "Y\n", 2);
		return 0;
	}
}
