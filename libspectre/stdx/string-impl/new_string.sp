import <"stdx/string">
import <"std/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func type string* new_string(const char* c) {
			using std::string::strlen;
			using std::string::strcpy;
			using std::lib::malloc;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(strlen(c) + 1) as char*;
			strcpy(ret->c_string, c);
			ret->size = strlen(c);
			return ret;
		}
	}
}
