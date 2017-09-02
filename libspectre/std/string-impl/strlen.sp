import <"std/string">

namespace std {
	namespace string {

		func unsigned int strlen(const char* str) {
			char* iter = str as char*;
			while(iter@ as bool) iter = iter[1]$;
			return iter as unsigned int - str as unsigned int;
		}
	}
}
