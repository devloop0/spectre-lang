import <"std/lib">
import <"std/syscall">

namespace std {
	namespace lib {

		func void _Exit(int status) {
			return std::syscall::exit(status);
		}
	}
}
