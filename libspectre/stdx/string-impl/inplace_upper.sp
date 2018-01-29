import <"stdx/string">
import <"std/ctype">

namespace stdx {
	namespace string_impl {

		func type string* inplace_upper(type string* s1) {
			using std::ctype::toupper;
			
			for(int i = 0; i < s1->size; i++)
				s1->c_string[i] = toupper(s1->c_string[i]);
			return s1;
		}
	}
}
