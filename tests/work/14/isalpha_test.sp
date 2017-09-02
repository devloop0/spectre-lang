import <"std/syscall">
import <"std/ctype">
import <"std/lib">

func int main() {
	using std::syscall::direct_write;
	using std::lib::_Exit;
	using std::ctype::isalpha;
	if(!isalpha('c') || (((isalpha)))('5') || (!(((isalpha)))('C'))) {
		direct_write(1, "N\n", 2);
		_Exit(1);
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
