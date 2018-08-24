import <"stdx/iterator">
import <"stdx/vector">
import <"stdx/algorithm">

import "../test.sp"

using namespace stdx::vector;
using namespace stdx::iterator;
using namespace stdx::algorithm;

const unsigned int NUM = 1'000;

func bool is_even(byte* a) {
	return (a as int* @) % 2 == 0;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{int});
	
	for(unsigned int i = 0; i < NUM; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	if(vector::size(v) != NUM) fail();

	type iterator::iterator* s = vector::begin(v), e = vector::end(v);
	unsigned long count_even = algorithm::count(s, e, is_even);
	if(count_even != NUM / 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::rbegin(v), e = vector::rend(v);
	iterator::advance(s, 100);
	count_even = algorithm::count(s, e, is_even);
	if(count_even != (NUM - 100) / 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);
	
	success();
}
