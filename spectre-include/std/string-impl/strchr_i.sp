import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strchr_i(const char* str , const char c ) {
			unsigned int i = 0;
			while(str[i] != '\0' && str[i] != c) i++;
			return i;
		}
	}
}
