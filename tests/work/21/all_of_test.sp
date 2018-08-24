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

func bool is_even(byte* a) {
	return (a as int* @) % 2 == 0;
}

func byte* add(byte* a, byte* b) {
	(a as int* @) += (b as int* @);
	return a;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{int});

	for(unsigned int i = 1; i <= NUM; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	type iterator::iterator* s = vector::begin(v), e = vector::end(v);
	if(algorithm::all_of(s, e, is_even)) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::rbegin(v), e = vector::rend(v);
	algorithm::for_each(s, e, x2);
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::begin(v), e = vector::end(v);
	if(!algorithm::all_of(s, e, is_even)) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::begin(v), e = iterator::back(vector::end(v), 1);
	int init = 0;
	int res = algorithm::accumulate(s, e, init$ as byte*, add) as int* @;
	if(res != 2 * (NUM - 1) * (NUM - 1 + 1) / 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = vector::rbegin(v), e = iterator::back(vector::rend(v), 1);
	init = 0;
	res = algorithm::accumulate(s, e, init$ as byte*, add) as int* @;
	if(res != 2 * NUM * (NUM + 1) / 2 - 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	vector::delete_vector(v);
	success();
}
