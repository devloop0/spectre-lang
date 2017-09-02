import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isblank(char c) {
			return c == '\t' || c == ' ';
		}
	}
}
