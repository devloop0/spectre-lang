import <"std/syscall">
import <"std/string">

func int main() {
	using std::syscall::direct_write;
	using std::string::strlen;
	using std::string::strcat;
	using std::string::strcpy;

	char* str = stk char(80);
	strcpy(str, "these ");
	strcat(str, "strings ");
	strcat(str, "are ");
	strcat(str, "concatenated!!\n");
	direct_write(1, str, strlen(str));
	return strlen(str);
}
