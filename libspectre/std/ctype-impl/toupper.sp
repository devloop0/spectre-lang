import <"std/ctype">

namespace std {
	namespace ctype {
	
		func char toupper(char c) {
			if(!islower(c)) return c;
			return (c - 'a') + 'A';
		}
	}
}
