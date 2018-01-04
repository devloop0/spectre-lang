import <"std/string">
import <"std/syscall">

func int main() {
	using namespace std::string;
	using namespace std::syscall;

	const char* str1 = "ABCDDBABCDBDEJHINLKJN1", str2 = "ABCD";
	if(strspn(str1, str2) != 12 || strspn("AAAB", "AB") != 4 || strspn("WLKRJWNER", "A") != 0) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	else {
		direct_write(1, "Y\n", 2);
		return 0;
	}
	direct_write(1, "N\n", 2);
	return 1;
}
