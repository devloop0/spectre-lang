import <"std/string">
import <"std/syscall">

func int main() {
	char* buffer1 = "DWgaOtP12df0";
	char* buffer2 = "DWGAOTP12DF0";

	int n = std::string::memcmp(buffer1 as byte*, buffer2 as byte*, std::string::strlen(buffer1) + 1);
	
	char* c = n > 0 ? "greater" : (n < 0 ? "less" : "same");
	if(std::string::strcmp(c, "greater") != 0) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
