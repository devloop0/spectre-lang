import <"stdx/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* concatch(type string* s1, const char c) {
			using std::lib::realloc;
			
			s1->c_string = realloc(s1->c_string as byte*, sizeof{char} * (s1->size + 1 + 1)) as char*;
			s1->c_string[s1->size] = c;
			s1->c_string[++s1->size] = 0;
			return s1;
		}
	}
}
