import <"std/string">

namespace std {
	namespace string {

		func char* strpbrk(const char* str1, const char* str2) {
			bool hit = false;
			char* str = str1 as char*;
			while(str@ != 0) {
				unsigned int j = 0;
				while(str2[j] != 0) {
					if(str@ == str2[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				str = str[1]$;
			}
			return hit ? str : NULL;
		}
	}
}
