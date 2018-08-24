import <"stdx/algorithm">
import <"stdx/iterator">
import <"stdx/vector">

import "../test.sp"

using namespace stdx::iterator;
using namespace stdx::vector;
using namespace stdx::algorithm;

func bool is_odd(byte* b) {
	return b as int* @ % 2 == 1;
}

func bool is_even(byte* b) {
	return b as int* @ % 2 == 0;
}

func bool is_50(byte* b) {
	return b as int* @ == 50;
}

func void x2(byte* b) {
	(b as int* @) *= 2;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{int});

	for(int i = 0; i < 100; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	type iterator::iterator* s = vector::begin(v), e = vector::end(v);
	type iterator::iterator* it = algorithm::find_if(s, e, is_odd);
	if(iterator::deref(it) as int* @ != 1) fail();

	if(iterator::iterator_sub(s, it) != -1) fail();
	iterator::delete_iterator(it);

	algorithm::for_each(s, e, x2);

	it = algorithm::find_if(s, e, is_odd);
	type iterator::iterator* it2 = algorithm::find_if_not(s, e, is_even);
	if(iterator::neq(it, it2) || iterator::neq(it, e)) fail();
	iterator::delete_iterator(it), iterator::delete_iterator(it2);

	it = algorithm::find_if(s, e, is_50);
	if(iterator::iterator_sub(it, s) != 25) fail();
	if(iterator::deref(it) as int* @ != 50) fail();
	iterator::delete_iterator(it), iterator::delete_iterator(s), iterator::delete_iterator(e);

	vector::delete_vector(v);
	success();
}
