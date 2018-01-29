import <"stdx/string">
import <"std/string">

namespace stdx {
	namespace string_impl {

		func int find_range(const type string* s1, const type string* s2, unsigned int start, unsigned int end) {
			if(end <= start) return -1;
			if(start >= s1->size || end > s1->size) return -1;

			using std::string::strstr;

			type string* temp = s1 as type string*;
			char old = temp->c_string[end];
			temp->c_string[end] = 0;
			const char* res = strstr(temp->c_string[start]$, s2->c_string);
			temp->c_string[end] = old;
			if(!res as bool) return -1;
			return res as unsigned int - s1->c_string as unsigned int;
		}
	}
}
