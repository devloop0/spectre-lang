import <"std/string">

namespace std {
	namespace string {
	
		func char* strncpy(char* str1 __rfuncarg__, const char* str2 __rfuncarg__,
			const unsigned int n __rfuncarg__) {
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
