import <"stdx/string">
import <"std/lib">

namespace stdx {
	namespace string_impl {

		func void delete_string(type string* s) {
			using std::lib::free;
			
			free(s->c_string as byte*);
			free(s as byte*);
		}
	}
}
