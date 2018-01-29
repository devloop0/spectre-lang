import <"stdx/string">
import <"std/string">

namespace stdx {
	namespace string_impl {

		func int find(const type string* s1, const type string* s2) {
			using std::string::strstr;

			if(s1->size < s2->size) return -1;
			const char* res = strstr(s1->c_string, s2->c_string);
			if(!res as bool) return -1;
			return res as unsigned int - s1->c_string as unsigned int;
		}
	}
}
