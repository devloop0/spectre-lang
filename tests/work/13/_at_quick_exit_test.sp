import <"std/lib">
import <"std/syscall">

using std::syscall::direct_write;
using std::lib::at_quick_exit;
using std::lib::quick_exit;

func void f1() { direct_write(1, "1\n", 2); }
func void f2() { direct_write(1, "2\n", 2); }

func int main() {
	at_quick_exit(f1);
	at_quick_exit(f2);
	quick_exit(0);
	f1();
}
