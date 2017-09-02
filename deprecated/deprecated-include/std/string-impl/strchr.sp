import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		func char* strchr(char* str , const char c ) {
			using std::core::char_nullptr;
			while(str@ != c) {
				if(str@ == '\0') return char_nullptr;
				str = str[1]$;
			}
			return str;
		}
	}
}
