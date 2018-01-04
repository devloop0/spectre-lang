import <"std/string">

namespace std {
	namespace string {

		func char* strncat(char* str1, const char* str2, const unsigned int n) {
			unsigned int i = 0, j = 0;
			while(str1[i] != 0) i++;
			while(j < n && str2[j] != 0) {
				str1[i + j] = str2[j];
				j++;
			}
			if(j < n && str2[j] == 0) str1[i + j] = 0;
			return str1;
		}
	}
}
