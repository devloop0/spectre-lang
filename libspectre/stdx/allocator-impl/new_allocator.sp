import <"stdx/allocator">

namespace stdx {
	namespace allocator_impl {

		func type allocator* new_allocator(fn byte*(const unsigned int) m,
			fn void(const byte*) f, fn byte*(byte*, const unsigned int) r) {
			type allocator* ret = m(sizeof{type allocator}) as type allocator*;
			ret->malloc = m;
			ret->free = f;
			ret->realloc = r;
		}
	}
}
