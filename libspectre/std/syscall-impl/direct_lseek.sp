import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_lseek(int fd, int offset, int whence) {
			return do_syscall3(LINUX_ARM_LSEEK, fd, offset, whence);
		}
	}
}
