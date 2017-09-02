import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isspace(char c) {
			return c == ' ' || c == '\t' || c == '\n' ||
				c == '\v' || c == '\f' || c == '\r';
		}
	}
}
