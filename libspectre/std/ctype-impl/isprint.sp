import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isprint(char c) {
			return isalnum(c) || ispunct(c) || isspace(c);
		}
	}
}
