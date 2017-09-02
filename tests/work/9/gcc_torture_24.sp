import <"std/syscall">
import <"std/string">

unsigned char* lookup_table = resv unsigned char(257);

func[static] int build_lookup(unsigned char* pattern) {
	int m = std::string::strlen(pattern) - 1;
	std::string::memset(lookup_table as byte*, ++m, 257);
	return m;
}

func int main() {
	if(build_lookup("bind") != 4) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
