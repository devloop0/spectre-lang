import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* chconcat(const char c, type string* s1) {
			using std::string::strlen;
			using std::string::memmove;
			using std::lib::realloc;

			s1->c_string = realloc(s1->c_string as byte*, (s1->size + 1 + 1) * sizeof{char}) as char*;
			memmove(s1->c_string[1]$ as byte*, s1->c_string as byte*, (s1->size + 1) * sizeof{char});
			s1->c_string@ = c;
			s1->size++;
			return s1;
		}
	}
}
