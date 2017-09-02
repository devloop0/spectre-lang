import <"std/string">
import <"std/syscall">

func int main() {
	char* str = "memmove can be very useful......";
	std::string::memmove(str[20]$ as byte*, str[15]$ as byte*, 11);
	if(std::string::strcmp(str, "memmove can be very very useful.") as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	char* dest = "oldstring";
	const char* src = "newstring";
	if(std::string::strcmp(dest, "oldstring") as bool ||
		std::string::strcmp(src, "newstring") as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	dest = std::string::memmove(dest as byte*, src as byte*, 9) as char*;
	if(std::string::strcmp(dest, "newstring") as bool ||
		std::string::strcmp(src, "newstring") as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
