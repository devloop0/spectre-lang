import <"std/string">

namespace std {
	namespace string {
		
		func const unsigned int strstr_i(const char* str1 , const char* str2 ) {
			unsigned int i = 0;
			while(str1[i] != '\0') {
				unsigned int j = 0;
				while(str1[i + j] != '\0' && str2[j] != '\0' && str1[i + j] == str2[j]) j++;
				if(str2[j] == '\0') break;
				i++;
			}
			return i;
		}
	}
}
