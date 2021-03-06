import <"stdx/string">
import <"std/lib">
import <"std/string">

namespace stdx {
	namespace string_impl {

		func type string* chadd(const char c, const type string* s1) {
			using std::lib::malloc;
			using std::string::strcpy;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(sizeof{char} * (s1->size + 1 + 1)) as char*;
			ret->size = s1->size + 1;
			strcpy(ret->c_string[1]$, s1->c_string);
			ret->c_string@ = c;
			return ret;
		}
	}
}
