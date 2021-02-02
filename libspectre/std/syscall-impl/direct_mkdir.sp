import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_mkdir(const char* pathname, unsigned int mode) {
			return do_syscall2(LINUX_ARM_MKDIR, pathname as int, mode);
		}
	}
}
