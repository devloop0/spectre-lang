import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_close(int fd) {
			int ret, status;
			__asm__ (
				fd "$4" :
				"lw $4, 0($4)" :
				LINUX_MIPS_CLOSE "$2" :
				"syscall" :
				ret "$8" :
				"sw $2, 0($8)" :
				status "$8" :
				"sw $7, 0($8)"
			);
			if(status != 0) return -1;
			return ret;
		}
	}
}
