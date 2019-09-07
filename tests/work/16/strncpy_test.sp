import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	char* src = stk char(40), dest = stk char(16);
	memset(dest as byte*, 0, 16);
	strcpy(src, "This is spectre-lang");
	strncpy(dest, src, 15);
	if(!strcmp(dest, "This is spectre") as bool) {
		direct_write(1, "Y\n", 2);
		return 0;
	}
	else {
		direct_write(1, "N\n", 2);
		return 1;
	}
}
