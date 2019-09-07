import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_fstat(int fd, type stat* statbuf) {
			return do_syscall2(LINUX_ARM_FSTAT, fd, statbuf as int);
		}
	}
}
