import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strpbrk_i(const char* str1 , const char* str2 ) {
			unsigned int ret = 0;
			while(str1[ret] != '\0') {
				unsigned int j = 0;
				bool hit = false;
				while(str2[j] != '\0') {
					if(str1[ret] == str2[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				ret++;
			}
			return ret;
		}
	}
}
