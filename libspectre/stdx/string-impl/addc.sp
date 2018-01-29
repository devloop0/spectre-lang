import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* addc(const type string* s1, const char* c) {
			using std::lib::malloc;
			using std::string::strcpy;
			using std::string::strcat;
			using std::string::strlen;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(s1->size + strlen(c) + 1) as char*;
			strcpy(ret->c_string, s1->c_string);
			strcat(ret->c_string, c);
			ret->size = s1->size + strlen(c);

			return ret;
		}
	}
}
