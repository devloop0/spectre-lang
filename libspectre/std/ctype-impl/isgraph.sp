import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isgraph(char c) {
			return isalnum(c) || ispunct(c);
		}
	}
}
