import <"stdx/allocator">

namespace stdx {
	namespace allocator_impl {

		func byte* reallocate(type allocator* a, byte* b, const unsigned int sz) {
			return a->realloc(b, sz);
		}
	}
}
