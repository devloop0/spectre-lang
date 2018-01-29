import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_close(int fd) {
			int ret;
			__asm__ (
				fd "$4" :
				"lw $4, 0($4)" :
				LINUX_MIPS_CLOSE "$2" :
				"lw $2, 0($2)" :
				"syscall" :
				ret "$8" :
				"sw $2, 0($8)"
			);
			return ret;
		}
	}
}
