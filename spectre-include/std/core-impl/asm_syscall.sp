import <"std/core">

namespace std {
	namespace core {

		func const bool asm_syscall(const unsigned int u) {
			__asm__ (
				u "$2" :
				"lw $2, 0($2)" :
				"syscall"
			);
			return true;
		}
	}
}
