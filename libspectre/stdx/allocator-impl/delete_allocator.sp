import <"stdx/allocator">

namespace stdx {
	namespace allocator_impl {

		func void delete_allocator(type allocator* a) {
			a->free(a as byte*);
		}
	}
}
