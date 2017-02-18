import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strnchr_i(const char* str __rfuncarg__, const char c __rfuncarg__,
			const unsigned int n __rfuncarg__) {
			unsigned int counter = 0;
			while(counter < n && str[counter] != c && str[counter] != '\0') counter++;
			return counter;
		}
	}
}
