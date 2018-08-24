import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_open2(const char* pathname, int flags, unsigned int mode) {
			int ret, status;
			__asm__ (
				pathname "$4" :
				"lw $4, 0($4)" :
				flags "$5" :
				"lw $5, 0($5)" :
				mode "$6" :
				"lw $6, 0($6)" :
				LINUX_MIPS_OPEN "$2" :
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
