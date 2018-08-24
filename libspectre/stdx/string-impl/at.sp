import <"stdx/string">

namespace stdx {
	namespace string_impl {

		func char at(type string* s1, unsigned int index) {
			if(index >= s1->size)
				return -1;
			return s1->c_string[index];
		}
	}
}
