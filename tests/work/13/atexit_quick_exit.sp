import <"std/syscall">
import <"std/lib">

using std::syscall::direct_write;
using std::lib::atexit;
using std::lib::quick_exit;

func void f1() { direct_write(1, "N\n", 2); }

func int main() {
	atexit(f1);
	quick_exit(0);
	return 0;
}
