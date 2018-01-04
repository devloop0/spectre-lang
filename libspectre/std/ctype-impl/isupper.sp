import <"std/ctype">

namespace std {
	namespace ctype {

		func bool isupper(char c) {
			return 'A' <= c && c <= 'Z';
		}
	}
}
