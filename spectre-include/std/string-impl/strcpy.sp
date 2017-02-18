import <"std/string">

namespace std {
	namespace string {

		func char* strcpy(char* str1 __rfuncarg__, const char* str2 __rfuncarg__) {
			unsigned int counter = 0;
			while((str1[counter] = str2[counter]) != '\0') counter++;
			return str1;
		}
	}
}
