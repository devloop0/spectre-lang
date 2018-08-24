import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_lseek(int fd, int offset, int whence) {
			int ret, status;
			__asm__ (
				LINUX_MIPS_LSEEK "$2" :
				fd "$4" :
				"lw $4, 0($4)" :
				offset "$5" :
				"lw $5, 0($5)" :
				whence "$6" :
				"lw $6, 0($6)" :
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
