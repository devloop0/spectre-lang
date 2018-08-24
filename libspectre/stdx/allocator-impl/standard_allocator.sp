import <"stdx/allocator">
import <"std/lib">

namespace stdx {
	namespace allocator_impl {
		
		func type allocator* standard_allocator() {
			return new_allocator(std::lib::malloc, std::lib::free, std::lib::realloc);
		}
	}
}
