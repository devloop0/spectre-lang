import <"std/syscall">
import <"std/string">

double* m = [7709179928849219.0, 771 as double];

func int main() {
	m[1]-- as bool ? m[0] *= 2, main() : (std::syscall::direct_write(1, m as char*, std::string::strlen(m as char*)), 1);
	char* n = "\nLol jk, and big endian :/\n";
	//std::syscall::direct_write(1, n, std::string::strlen(n));
}
