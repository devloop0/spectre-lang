import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_open2(const char* pathname, int flags, unsigned int mode) {
			return do_syscall3(LINUX_ARM_OPEN, pathname as int, flags, mode);
		}
	}
}
