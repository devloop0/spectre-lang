import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	char* src = stk char(50), dest = stk char(50);
	strcpy(src, "This is source");
	strcpy(dest, "This is destination");

	strncat(dest, src, 15);
	
	if(strcmp(dest, "This is destinationThis is source") == 0) {
		direct_write(1, "Y\n", 2);
		return 0;
	}
	else {
		direct_write(1, "N\n", 2);
		return 1;
	}
}
