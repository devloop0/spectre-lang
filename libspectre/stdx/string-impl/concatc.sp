import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {
	
		func type string* concatc(type string* s1, const char* c) {
			using std::lib::realloc;
			using std::string::strcat;
			using std::string::strlen;

			s1->c_string = realloc(s1->c_string as byte*,
				s1->size + strlen(c) + 1) as char*;
			strcat(s1->c_string, c);
			s1->size += strlen(c);
			return s1;
		}
	}
}
