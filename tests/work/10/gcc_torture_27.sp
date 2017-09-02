import <"std/syscall">
import <"std/string">

func int main() {
	if(std::string::strcmp("X", "") < 0) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
