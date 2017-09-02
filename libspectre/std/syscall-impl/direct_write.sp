import <"std/syscall">

namespace std {
	namespace syscall {
	
		func void direct_write(int fd, const char* buf, const unsigned int count) {
			__asm__ (
				fd "$4" :
				"lw $4, 0($4)" :
				buf "$5" :
				"lw $5, 0($5)" :
				count "$6" :
				"lw $6, 0($6)" :
				LINUX_MIPS_WRITE "$2" :
				"lw $2, 0($2)" :
				"syscall"
			);
		}
	}
}
