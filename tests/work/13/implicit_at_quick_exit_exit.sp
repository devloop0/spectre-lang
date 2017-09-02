import <"std/syscall">
import <"std/lib">

using std::syscall::direct_write;

func void f1() { return direct_write(1, "N\n", 2); }

func int main() {
	std::lib::at_quick_exit(f1);
}
