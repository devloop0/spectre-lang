import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	if(strcmp(strrchr("https://www.google.com/", '.'), ".com/") != 0 ||
		strrchr("alkjafd", '.') != NULL) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else {
		direct_write(1, "Y\n", 2);
		return 0;
	}
	direct_write(1, "N\n", 2);
	return 0;
}
