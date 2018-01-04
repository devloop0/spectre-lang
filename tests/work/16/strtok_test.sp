import <"std/string">
import <"std/syscall">

func void fail() {
	std::syscall::direct_write(1, "N\n", 2);
	std::syscall::exit(1);
}

func void success() {
	std::syscall::direct_write(1, "Y\n", 2);
	std::syscall::exit(0);
}

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	char* url = "https://www.mail.google.com/";
	char** url_parts = ["https", "www", "mail", "google", "com"];
	char* delims = ":/.";
	char* token = strtok(url, delims);
	int i = 0;
	while(token as bool) {
		if(strcmp(token, url_parts[i++]) != 0) fail();
		token = strtok(NULL, delims);
	}
	success();
}
