import <"std/string">

namespace std {
	namespace string {

		func char* strcpy(char* str1, const char* str2) {
			char* s2 = str2 as char*, orig = str1;
			while((str1@ = s2@) as bool)
				str1 = str1[1]$, s2 = s2[1]$;
			return orig;
		}
	}
}
