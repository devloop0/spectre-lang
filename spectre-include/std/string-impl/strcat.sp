import <"std/string">

namespace std {
	namespace string {

		func char* strcat(char* str1 , const char* str2 ) {
			unsigned int i = 0, j = 0;
			while(str1[i] != '\0') i++;
			while((str1[i + j] = str2[j]) != '\0') j++;
			return str1;
		}
	}
}
