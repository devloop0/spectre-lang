import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strnchr_i(const char* str , const char c ,
			const unsigned int n ) {
			unsigned int counter = 0;
			while(counter < n && str[counter] != c && str[counter] != '\0') counter++;
			return counter;
		}
	}
}
