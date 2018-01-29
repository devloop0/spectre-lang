import <"stdx/string">

namespace stdx {
	namespace string_impl {

		func unsigned int length(const type string* s1) {
			return s1->size;
		}
	}
}
