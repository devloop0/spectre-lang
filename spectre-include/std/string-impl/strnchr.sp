import <"std/string">
import <"std/core">

namespace std {
	namespace string {

		func char* strnchr(char* str , const char c ,
			const unsigned int n ) {
			using std::core::char_nullptr;
			if(n == 0) return char_nullptr;
			unsigned int counter = 0;
			while(str[counter] != c) {
				if(str[counter] == '\0') return char_nullptr;
				counter++;
				if(counter >= n) return char_nullptr;
			}
			return str[counter]$;
		}
	}
}
