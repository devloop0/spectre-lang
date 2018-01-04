import <"std/string">

namespace std {
	namespace string {

		func char* strrchr(const char* str1, const char c) {
			char* str = str1 as char*;
			bool hit = false;
			unsigned int i = 0, last_occ = 0;
			while(str[i] != 0) {
				if(str[i] == c) hit = true, last_occ = i;
				i++;
			}
			return hit ? str[last_occ]$ : NULL;
		}
	}
}
