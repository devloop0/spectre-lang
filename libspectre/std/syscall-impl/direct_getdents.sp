import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_getdents(unsigned int fd, byte* stream, unsigned int count) {
			return do_syscall3(LINUX_ARM_GETDENTS, fd, stream as int, count);
		}
	}
}
