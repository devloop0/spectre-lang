import <"std/syscall">

func int main() {
	namespace ssys = std::syscall;
	ssys::direct_write(1, "Y\n", 2);
	return 0;
}
