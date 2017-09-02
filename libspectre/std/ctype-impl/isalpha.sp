import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isalpha(char c) {
			return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
		}
	}
}
