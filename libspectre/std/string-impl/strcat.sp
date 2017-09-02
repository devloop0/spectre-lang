import <"std/string">

namespace std {
	namespace string {
		
		func char* strcat(char* str1, const char* str2) {
			char* orig = str1, s2 = str2 as char*;
			while(str1@ as bool) str1 = str1[1]$;
			while((str1@ = s2@) as bool)
				str1 = str1[1]$, s2 = s2[1]$;
			return orig;
		}
	}
}
