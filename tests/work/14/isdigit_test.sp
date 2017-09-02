import <"std/ctype">
import <"std/lib">
import <"std/syscall">

func int main() {
	using std::ctype::isdigit;
	using std::syscall::direct_write;
	using std::lib::_Exit;
	if(!isdigit('2') || isdigit('h')) {
		direct_write(1, "N\n", 2);
		_Exit(1);
	}
	direct_write(1, "Y\n", 2);
}
