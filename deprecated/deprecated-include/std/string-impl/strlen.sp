import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strlen(const char* str ) {
			unsigned int len = 0;
			while(str[len++] != '\0') {}
			return len - 1;
		}
	}
}
