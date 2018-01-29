import <"stdx/string">

namespace stdx {
	namespace string_impl {
	
		func char* data(const type string* s1) {
			return s1->c_string as char*;
		}
	}
}
