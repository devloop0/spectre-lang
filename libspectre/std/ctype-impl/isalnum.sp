import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isalnum(char c) {
			return isalpha(c) || isdigit(c);
		}
	}
}
