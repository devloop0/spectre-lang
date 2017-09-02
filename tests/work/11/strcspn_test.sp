import <"std/syscall">
import <"std/string">

using std::syscall::direct_write;
using std::string::strcspn;

func int main() {
	int len;
	const char* str1 = "ABCDEF4960910";
	const char* str2 = "013";
	if(strcspn(str1, str2) + 1 != 10) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
