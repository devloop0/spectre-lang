import <"std/string">

namespace std {
	namespace string {

		func unsigned int strcspn(const char* str1, const char* str2) {
			char* iter1 = str1 as char*;
			while(iter1@ as bool) {
				char* iter2 = str2 as char*;
				while(iter2@ as bool) {
					if(iter1@ == iter2@) break;
					iter2 = iter2[1]$;
				}
				if(iter2@ as bool) break;
				iter1 = iter1[1]$;
			}
			return iter1 as unsigned int - str1 as unsigned int;
		}
	}
}
