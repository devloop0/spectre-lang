import <"stdx/vector">
import <"stdx/iterator">
import <"stdx/algorithm">

import "../test.sp"

using namespace stdx::vector;
using namespace stdx::iterator;
using namespace stdx::algorithm;

const unsigned int NUM = 100;

func void x2(byte* a) {
	(a as int* @) *= 2;
}

func bool is_odd(byte* a) {
	return ((a as int* @) % 2) as bool;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{int});
	for(unsigned int i = 0; i < 100; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	if(vector::size(v) != NUM) fail();

	type iterator::iterator* s = vector::begin(v), e = vector::end(v);
	if(algorithm::none_of(s, e, is_odd)) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::rbegin(v), e = vector::rend(v);
	algorithm::for_each(s, e, x2);
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::begin(v), e = vector::end(v);
	if(!algorithm::none_of(s, e, is_odd)) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	vector::delete_vector(v);
	success();
}
