import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_syncfs(int fd) {
			return do_syscall1(LINUX_ARM_SYNCFS, fd);
		}
	}
}
