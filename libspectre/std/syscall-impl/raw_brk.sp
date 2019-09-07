import <"std/syscall">

namespace std {
	namespace syscall {

		func byte* raw_brk(const byte* data) {
			return do_syscall1(LINUX_ARM_BRK, data as int) as byte*;
		}
	}
}
