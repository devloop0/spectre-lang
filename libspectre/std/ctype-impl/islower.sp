import <"std/ctype">

namespace std {
	namespace ctype {
	
		func bool islower(char c) {
			return 'a' <= c && c <= 'z';
		}
	}
}
