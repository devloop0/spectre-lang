import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		func char* strnpbrk(char* str1 , const char* str2 ,
			const unsigned int n ) {
			bool hit = false;
			unsigned int ret = 0;
			while(str1[ret] != '\0' && ret < n) {
				unsigned int j = 0;
				while(str2[j] != '\0') {
					if(str1[ret] == str2[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				ret++;
			}
			return hit ? str1[ret]$ : std::core::char_nullptr;
		}
	}
}
