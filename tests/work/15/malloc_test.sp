import <"std/lib">
import <"std/syscall">
import <"std/string">

func int main() {
	using std::lib::malloc;
	using std::syscall::direct_write;
	using std::string::strlen;
	using std::string::strcpy;
	using std::string::strcat;
	
	char* c = malloc(100) as char*, c2 = malloc(100) as char*;
	strcpy(c, "nikhil implemented malloc :^)\n");
	direct_write(1, c, strlen(c));
	strcpy(c2, "twice\n");
	direct_write(1, c2, strlen(c2));
	strcat(c2, "thrice :)\n");
	direct_write(1, c2, strlen(c2));
}
