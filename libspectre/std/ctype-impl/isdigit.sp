import <"std/ctype">

namespace std {
	namespace ctype {
	
		func bool isdigit(char c) {
			return c >= '0' && c <= '9';
		}
	}
}
