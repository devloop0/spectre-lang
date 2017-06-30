import <"std/string">

namespace std {
	namespace string {
		
		func const signed int strncmp(const char* str1 , const char* str2 ,
			const unsigned int n ) {
			if(n == 0) return 0;
			unsigned int counter = 0;
			while(str1[counter] == str2[counter]) {
				if(str1[counter] == '\0' || counter == n - 1) return 0;
				counter++;
			}
			return str1[counter] < str2[counter] ? -1 : 1;
		}
	}
}
