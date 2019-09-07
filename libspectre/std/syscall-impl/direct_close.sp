import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_close(int fd) {
			return do_syscall1(LINUX_ARM_CLOSE, fd);
		}
	}
}
