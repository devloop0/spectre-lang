import <"std/string">
import <"std/syscall">

func int main() {
	using std::syscall::direct_write;
	using std::string::strcmp;
	using std::string::strlen;

	char* nikhil = "Nikhil", nidhi = "Nidhi";
	int res = strcmp(nidhi, nikhil);
	char* to_print = res > 0 ? "After\n" : (res < 0 ? "Before\n" : "Equal\n");
	direct_write(1, to_print, strlen(to_print));
	return 0;
}
