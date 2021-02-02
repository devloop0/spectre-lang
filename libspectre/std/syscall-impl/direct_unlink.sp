import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_unlink(const char* pathname) {
			return do_syscall1(LINUX_ARM_UNLINK, pathname as int);
		}
	}
}
