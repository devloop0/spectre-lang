import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_stat(const char* pathname, type stat* statbuf) {
			int ret, status;
			byte* tmp = stk byte(144);
			__asm__ (
				LINUX_MIPS_STAT "$2" :
				pathname "$4" :
				"lw $4, 0($4)" :
				tmp "$5" :
				"lw $5, 0($5)" :
				"syscall" :
				ret "$8" :
				"sw $2, 0($8)" :
				status "$8" :
				"sw $7, 0($8)"
			);
			if(status != 0) return -1;
			bytes_to_stat(tmp, statbuf);
			return ret;
		}
	}
}
