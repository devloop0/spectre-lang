import <"std/syscall">

namespace std {
	namespace syscall {
	
		func int direct_read(int fd, char* buf, const unsigned int count) {
			return do_syscall3(LINUX_ARM_READ, fd, buf as int, count);
		}
	}
}
