import <"std/syscall">

namespace std {
	namespace syscall {

		func void direct_sync() {
			do_syscall0(LINUX_ARM_SYNC);
		}
	}
}
