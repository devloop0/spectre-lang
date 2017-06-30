import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		func char* strpbrk(char* str , const char* str2 ) {
			bool hit = false;
			while(str@ != '\0') {
				unsigned int j = 0;
				while(str2[j] != '\0') {
					if(str@ == str2[j]) {
						hit = true;
						break;
					}
					j++;
				}
				if(hit) break;
				str = str[1]$;
			}
			return hit ? str : std::core::char_nullptr;
		}
	}
}
