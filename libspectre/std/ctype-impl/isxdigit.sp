import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isxdigit(char c) {
			return ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') ||
				('0' <= c && c <= '9');
		}
	}
}
