import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_chmod(const char* pathname, unsigned int mode) {
			return do_syscall2(LINUX_ARM_CHMOD, pathname as int, mode);
		}
	}
}
