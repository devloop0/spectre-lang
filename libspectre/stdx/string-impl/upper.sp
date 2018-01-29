import <"stdx/string">
import <"std/ctype">
import <"std/lib">
import <"std/string">

namespace stdx {
	namespace string_impl {
		
		func type string* upper(const type string* s1) {
			using std::lib::malloc;
			using std::ctype::toupper;
			using std::string::strcpy;

			type string* ret = malloc(sizeof{type string}) as type string*;
			ret->c_string = malloc(s1->size + 1) as char*;
			strcpy(ret->c_string, s1->c_string);
			for(int i = 0; i < s1->size; i++)
				ret->c_string[i] = toupper(s1->c_string[i]);
			ret->size = s1->size;
			return ret;
		}
	}
}
