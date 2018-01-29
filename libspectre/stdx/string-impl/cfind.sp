import <"stdx/string">
import <"std/string">

namespace stdx {
	namespace string_impl {

		func int cfind(const type string* s1, const char* c) {
			using std::string::strstr;

			const char* res = strstr(s1->c_string, c);
			if(!res as bool) return -1;
			return res as unsigned int - s1->c_string as unsigned int;
		}
	}
}
