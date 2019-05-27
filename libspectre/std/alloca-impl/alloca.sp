import <"std/alloca">
import <"std/lib">

namespace std {
	namespace lib {

		// The compiler currently doesn't support variable length arrays
		// since the bottom of the stack is a fixed size and used for function arguments.
		// Fallback to malloc.
		func byte* alloca(const unsigned int sz) {
			return malloc(sz);
		}
	}
}
