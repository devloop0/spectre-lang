import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_open(const char* pathname, int flags) {
			return do_syscall2(LINUX_ARM_OPEN, pathname as int, flags);
		}
	}
}
