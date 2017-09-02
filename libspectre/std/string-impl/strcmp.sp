import <"std/string">

namespace std {
	namespace string {
	
		func int strcmp(const char* str1, const char* str2) {
			char* s1 = str1 as char*, s2 = str2 as char*;
			while(s1@ == s2@) {
				if(s1@ == 0) return 0;
				s1 = s1[1]$, s2 = s2[1]$;
			}
			return s1@ - s2@;
		}
	}
}
