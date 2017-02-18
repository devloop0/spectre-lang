import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		func const bool print_ln() {
			__asm__ ( "addiu $4, $0, '\n'" );
			return std::core::asm_syscall(11);
		}
	}
}
