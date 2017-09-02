import <"std/lib">
import <"std/syscall">

func void f1() { std::syscall::direct_write(1, "1\n", 2); }
func void f2() { std::syscall::direct_write(1, "2\n", 2); }
func void f3() { std::syscall::direct_write(1, "3\n", 2); }

func int main() {
	std::lib::atexit(f1);
	std::lib::atexit(f2);
	std::lib::atexit(f3);
}
