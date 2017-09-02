import <"std/lib">
import <"std/syscall">

namespace std {
	namespace lib {

		func void abort() {
			return std::syscall::exit(EXIT_FAILURE);
		}
	}
}
