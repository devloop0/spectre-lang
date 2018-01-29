import <"stdx/vector">

namespace stdx {
	namespace vector_impl {
	
		func unsigned int capacity(const type vector* v) {
			return v->capacity;
		}
	}
}

