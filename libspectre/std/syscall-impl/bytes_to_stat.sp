import <"std/syscall">
import <"std/string">

namespace std {
	namespace syscall {

		func void bytes_to_stat(byte* stream, type stat* statbuf) {
			std::string::memcpy(statbuf as byte*, stream as byte*, sizeof{type stat});
		}
	}
}
