import <"stdx/allocator">

namespace stdx {
	namespace allocator_impl {
	
		func void deallocate(type allocator* a, const byte* b) {
			a->free(b);
		}
	}
}
