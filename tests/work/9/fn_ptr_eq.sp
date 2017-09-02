import <"std/syscall">

func void f() {}
func void g() {}

func int main() {
	fn void() temp = f;
	if(temp != f || f == g) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
}
