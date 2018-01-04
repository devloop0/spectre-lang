import <"std/syscall">

namespace std {
	namespace syscall {

		func byte* raw_brk(const byte* data) {
			byte* ret;
			__asm__ (
				"addiu $2, $0, 4045" :
				data "$4" :
				"lw $4, 0($4)" :
				"syscall" :
				ret "$8" :
				"sw $2, 0($8)"
			);
			return ret;
		}
	}
}
