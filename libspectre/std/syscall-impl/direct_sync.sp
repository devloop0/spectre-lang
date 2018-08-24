import <"std/syscall">

namespace std {
	namespace syscall {

		func void direct_sync() {
			__asm__ (
				LINUX_MIPS_SYNC "$2" : 
				"syscall"
			);
		}
	}
}
