import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		func char* strstr(char* str1 , const char* str2 ) {
			bool hit = false;
			while(str1@ != '\0') {
				unsigned int i = 0;
				while(str2[i] != '\0' && str1[i] != '\0' && str1[i] == str2[i]) i++;
				if(str2[i] == '\0') {
					hit = true;
					break;
				}
				str1 = str1[1]$;
			}
			return hit ? str1 : std::core::char_nullptr;
		}
	}
}
