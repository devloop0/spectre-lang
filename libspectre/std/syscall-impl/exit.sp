import <"std/syscall">

namespace std {
	namespace syscall {

		func void exit(const int status_code) {
			__asm__ (
				status_code "$4" :
				"lw $4, 0($4)" :
				LINUX_MIPS_EXIT "$2" :
				"lw $2, 0($2)" :
				"syscall"
			);
		}
	}
}
