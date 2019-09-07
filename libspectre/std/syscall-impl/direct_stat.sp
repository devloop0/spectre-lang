import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_stat(const char* pathname, type stat* statbuf) {
			return do_syscall2(LINUX_ARM_STAT, pathname as int, statbuf as int);
		}
	}
}
