import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* add(const type string* s1, const type string* s2) {
			using std::lib::malloc;
			using std::string::strcpy;
			using std::string::strcat;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(s1->size + s2->size + 1) as char*;
			strcpy(ret->c_string, s1->c_string);
			strcat(ret->c_string, s2->c_string);
			ret->size = s1->size + s2->size;

			return ret;
		}
	}
}
