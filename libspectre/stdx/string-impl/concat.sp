import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* concat(type string* s1, const type string* s2) {
			using std::lib::realloc;
			using std::string::strcat;

			s1->c_string = realloc(s1->c_string as byte*,
				s1->size + s2->size + 1) as char*;
			strcat(s1->c_string, s2->c_string);
			s1->size += s2->size;
			return s1;
		}
	}
}
