import <"std/string">

namespace std {
	namespace string {
	
		func char* strncpy(char* str1 , const char* str2 ,
			const unsigned int n ) {
			unsigned int counter = 0;
			while(counter < n && str2[counter] != '\0') {
				str1[counter] = str2[counter];
				counter++;
			}
			if(counter < n && str2[counter] == '\0') str1[counter] = '\0';
			return str1;
		}
	}
}
