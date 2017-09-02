import <"std/syscall">
import <"std/machine">
import <"std/string">

func int main() {
	using std::string::strlen;
	using std::syscall::direct_write;
	using namespace std::machine;
	char* little_endian_to_write = little_endian() ?
		"This machine is little endian.\n" :
		"This machine isn't little endian.\n";
	char* big_endian_to_write = big_endian() ?
		"This machine is big endian.\n" :
		"This machine isn't big endian.\n";
	char* middle_endian_to_write = middle_endian() ?
		"This machine is middle endian.\n" :
		"This machine isn't middle endian.\n";
	direct_write(1, little_endian_to_write, strlen(little_endian_to_write));
	direct_write(1, big_endian_to_write, strlen(big_endian_to_write));
	direct_write(1, middle_endian_to_write, strlen(middle_endian_to_write));

	return bits();
}
