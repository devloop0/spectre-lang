import <"std/string">

namespace std {
	namespace string {
	
		func const signed int strcmp(const char* str1 , const char* str2 ) {
			unsigned int counter = 0;
			while(str1[counter] == str2[counter])
				if(str1[counter++] == '\0') return 0;
			return str1[counter] < str2[counter] ? -1 : 1;
		}
	}
}
