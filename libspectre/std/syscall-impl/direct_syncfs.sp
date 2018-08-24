import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_syncfs(int fd) {
			int ret, status;
			__asm__ (
				LINUX_MIPS_SYNCFS "$2" :
				fd "$4" :
				"lw $4, 0($4)" :
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
