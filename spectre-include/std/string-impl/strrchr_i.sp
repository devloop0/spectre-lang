import <"std/string">

namespace std {
	namespace string {

		func const unsigned int strrchr_i(const char* str __rfuncarg__, const char c __rfuncarg__) {
			bool hit = false;
			unsigned int i = 0, last_occ = 0;
			while(str[i] != '\0') {
				if(str[i] == c) hit = true, last_occ = i;
				i++;
			}
			return hit ? last_occ : i;
		}
	}
}
