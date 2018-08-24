import <"stdx/iterator">
import <"stdx/vector">
import <"stdx/algorithm">
import <"std/string">

import "../test.sp"

using namespace stdx::iterator;
using namespace stdx::vector;
using namespace stdx::algorithm;

func byte* add(byte* a, byte* b) {
	a as int* @ += b as int* @;
	return a;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{int});
	
	for(int i = 0; i < 100; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	type iterator::iterator* s = vector::begin(v), e = vector::end(v);
	int init = 0;
	if(algorithm::accumulate(e, e, init$ as byte*, add) as int* @ != 0) fail();
	if(init != 0) fail();
	if(algorithm::accumulate(s, e, init$ as byte*, add) as int* @ != 99 * (99 + 1) / 2) fail();
	if(algorithm::accumulate(s, e, init$ as byte*, add) as int* @ != 99 * (99 + 1) / 2 * 2) fail();

	iterator::delete_iterator(s), iterator::delete_iterator(e);
	vector::delete_vector(v);

	success();
}
