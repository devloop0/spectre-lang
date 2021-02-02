import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_truncate(const char* pathname, long length) {
			return do_syscall2(LINUX_ARM_TRUNCATE, pathname as int, length);
		}
	}
}
