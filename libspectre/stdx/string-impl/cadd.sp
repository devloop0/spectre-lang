import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* cadd(const char* c, const type string* s1) {
			using std::lib::malloc;
			using std::string::strcpy;
			using std::string::strcat;
			using std::string::strlen;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(strlen(c) + s1->size + 1) as char*;
			strcpy(ret->c_string, c);
			strcat(ret->c_string, s1->c_string);
			ret->size = strlen(c) + s1->size;

			return ret;
		}
	}
}
