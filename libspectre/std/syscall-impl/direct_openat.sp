import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_openat(int fd, const char* pathname, int flags) {
			int ret;
			__asm__ (
				fd "$4" :
				"lw $4, 0($4)" :
				pathname "$5" :
				"lw $5, 0($5)" :
				flags "$6" :
				"lw $6, 0($6)" :
				LINUX_MIPS_OPENAT "$2" :
				"lw $2, 0($2)" :
				"syscall" :
				ret "$8" :
				"sw $2, 0($8)"
			);
			return ret;
		}
	}
}
