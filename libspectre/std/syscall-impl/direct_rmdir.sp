import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_rmdir(const char* pathname) {
			return do_syscall1(LINUX_ARM_RMDIR, pathname as int);
		}
	}
}
