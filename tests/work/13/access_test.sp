import <"std/lib">
import <"std/hooks">
import <"std/syscall">

using std::syscall::direct_write;
using std::lib::exit;
using std::lib::atexit;

func void f1() { return direct_write(1, "1\n", 2); }
func void f2() { return direct_write(1, "2\n", 2); }
func void f3() { return direct_write(1, "3\n", 2); }

access type std::hooks::exit_data | ::std::hooks::ed;
access bool | std::hooks::ed_init;

func int main() {
	if(std::hooks::ed_init) {
		direct_write(1, "N\n", 2);
		std::syscall::exit(1);
	}
	atexit(f1);
	atexit(f2);
	atexit(f3);
	if(std::hooks::ed.sz != 3) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
