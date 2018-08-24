import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_openat2(int fd, const char* pathname, int flags, unsigned int mode) {
			int ret, status;
			__asm__ (
				fd "$4" :
				"lw $4, 0($4)" :
				pathname "$5" :
				"lw $5, 0($5)" :
				flags "$6" :
				"lw $6, 0($6)" :
				mode "$7" :
				"lw $7, 0($7)" :
				LINUX_MIPS_OPENAT "$2" :
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
