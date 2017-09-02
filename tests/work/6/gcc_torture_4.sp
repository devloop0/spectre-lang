import <"std/string">
import <"std/syscall">

func[static] int special_format(const char* fmt) {
	using std::string::strchr;
	using std::string::NULL;
	return (strchr(fmt, '*') != NULL || strchr(fmt, 'V') != NULL ||
		strchr(fmt, 'S') != NULL || strchr(fmt, 'n') != NULL) as int;
}

func int main() {
	if(special_format("ee") as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(!special_format("*e") as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
