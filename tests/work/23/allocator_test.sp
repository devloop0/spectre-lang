import <"stdx/allocator">

import "../test.sp"

func int main() {
	namespace xalloc = ::stdx::allocator::allocator;

	auto a = xalloc::standard_allocator();

	byte* b = xalloc::allocate(a, 1);
	b@ = 1;
	if(b@ != 1) fail();
	b = xalloc::reallocate(a, b, 2);
	if(b@ != 1) fail();
	xalloc::deallocate(a, b);
	
	success();
}
