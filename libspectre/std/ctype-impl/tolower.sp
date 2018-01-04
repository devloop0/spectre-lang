import <"std/ctype">

namespace std {
	namespace ctype {
		
		func char tolower(char c) {
			if(!isupper(c)) return c;
			return (c - 'A') + 'a';
		}
	}
}
