import <"stdx/vector">

namespace stdx {
	namespace vector_impl {

		func byte* data(const type vector* v) {
			return v->data;
		}
	}
}
