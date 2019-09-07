import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_openat2(int fd, const char* pathname, int flags, unsigned int mode) {
			return do_syscall4(LINUX_ARM_OPENAT, fd, pathname as int, flags, mode);
		}
	}
}
