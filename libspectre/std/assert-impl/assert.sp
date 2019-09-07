import <"std/syscall">
import <"std/string">
import <"std/lib">

namespace std {
	namespace assert {

		static constexpr unsigned int OUT_FD = 2;

		func void assert(bool b, char* msg) {
			if (!b) {
				std::syscall::direct_write(
					OUT_FD,
					msg,
					std::string::strlen(msg)
				);
				std::lib::abort();
			}
		}
	}
}
