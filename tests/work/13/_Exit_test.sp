import <"std/lib">
import <"std/syscall">

using std::lib::_Exit;
using std::syscall::direct_write;
using std::lib::at_quick_exit;
using std::lib::atexit;

func void f1() { return direct_write(1, "N\n", 2); }
func void f2() { return direct_write(1, "N2\n", 3); }

func int main() {
	atexit(f1);
	at_quick_exit(f2);
	_Exit(0);
}
