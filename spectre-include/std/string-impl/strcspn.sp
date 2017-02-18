import <"std/string">

namespace std {
	namespace string {
		
		func const unsigned int strcspn(const char* str1 __rfuncarg__, const char* str2 __rfuncarg__) {
			unsigned int i = 0;
			while(str1[i] != '\0') {
				unsigned int j = 0;
				bool hit = false;
				while(str2[j] != '\0') {
					if(str1[i] == str2[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				i++;
			}
			return i;
		}
	}
}
