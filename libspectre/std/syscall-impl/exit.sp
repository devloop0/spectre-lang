import <"std/syscall">

namespace std {
	namespace syscall {

		func void exit(const int status_code) {
			do_syscall1(LINUX_ARM_EXIT, status_code);
		}
	}
}
