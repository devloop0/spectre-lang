import <"std/syscall">

namespace std {
	namespace syscall {
	
		func int direct_write(int fd, const char* buf, const unsigned int count) {
			return do_syscall3(LINUX_ARM_WRITE, fd, buf as int, count);
		}
	}
}
