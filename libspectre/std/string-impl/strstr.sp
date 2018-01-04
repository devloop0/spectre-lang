import <"std/string">

namespace std {
	namespace string {
	
		func char* strstr(const char* str1, const char* str2) {
			char* str = str1 as char*;
			bool hit = false;
			while(str@ != 0) {
				unsigned int i = 0;
				while(str2[i] != 0 && str[i] != 0 &&
					str[i] == str2[i]) i++;
				if(str2[i] == 0) {
					hit = true;
					break;
				}
				str = str[1]$;
			}
			return hit ? str : NULL;
		}
	}
}
