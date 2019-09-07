import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_openat(int fd, const char* pathname, int flags) {
			return do_syscall3(LINUX_ARM_OPENAT, fd, pathname as int, flags);
		}
	}
}
