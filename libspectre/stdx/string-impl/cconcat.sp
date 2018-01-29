import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* cconcat(const char* c, type string* s1) {
			using std::string::strlen;
			using std::string::memmove;
			using std::lib::realloc;

			s1->c_string = realloc(s1->c_string as byte*,
				strlen(c) + s1->size + 1) as char*;
			memmove(s1->c_string[strlen(c)]$ as byte*,
				s1->c_string as byte*, s1->size + 1);
			memmove(s1->c_string as byte*, c as byte*, strlen(c));
			s1->size += strlen(c);
			return s1;
		}
	}
}
