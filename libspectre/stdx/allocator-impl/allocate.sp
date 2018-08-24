import <"stdx/allocator">

namespace stdx {
	namespace allocator_impl {

		func byte* allocate(type allocator* a, const unsigned int sz) {
			return a->malloc(sz);
		}
	}
}
