import <"std/string">

namespace std {
	namespace string {

		func char* strchr(const char* str, const char c) {
			char* iter = str as char*;
			while(iter@ != c) {
				if(!iter@ as bool) return NULL;
				iter = iter[1]$;
			}
			return iter;
		}
	}
}
