import <"stdx/vector">
import <"stdx/iterator">
import <"stdx/algorithm">

import "../test.sp"

using namespace stdx::vector;
using namespace stdx::iterator;
using namespace stdx::algorithm;

const unsigned int NUM = 1'000'000;

func byte* add(byte* a, byte* b) {
	a as double* @ += b as double* @;
	return a;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{double});
	
	for(double i = 1; i <= NUM; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	if(vector::size(v) != NUM) fail();

	type iterator::iterator* s = iterator::advance(vector::begin(v), 100), e = vector::end(v);
	double init = 0;
	double res = algorithm::accumulate(s, e, init$ as byte*, add) as double* @;
	if(res != 1e6 * (1e6 + 1) / 2 - 100 * (100 + 1) / 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	s = iterator::advance(vector::rbegin(v), 100), e = vector::rend(v);
	init = 0;
	res = algorithm::accumulate(s, e, init$ as byte*, add) as double* @;
	if(res != (1e6 - 100) * (1e6 - 100 + 1) / 2) fail();
	iterator::delete_iterator(s), iterator::delete_iterator(e);

	vector::delete_vector(v);
	success();
}
