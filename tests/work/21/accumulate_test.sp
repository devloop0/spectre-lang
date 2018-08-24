import <"stdx/vector">
import <"stdx/iterator">
import <"stdx/algorithm">
import <"std/string">
import <"std/lib">

import "../test.sp"

using namespace stdx::vector;
using namespace stdx::iterator;
using namespace stdx::algorithm;

const unsigned int NUM = 14;

func byte* mul(byte* a, byte* b) {
	a as double* @ *= b as double* @;
	return a;
}

unsigned int op = 0;

func byte* custom(byte* a, byte* b) {
	if((op++ % 2) as bool)
		a as double* @ -= b as double* @;
	else
		a as double* @ += b as double* @;
	return op = op % 2, a;
}

func int main() {
	type vector::vector* v = vector::new_vector(sizeof{double});
	
	for(double i = 1; i <= NUM; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	type iterator::iterator* s1 = vector::rbegin(v), e1 = vector::rend(v);
	double init = 1;
	double res = algorithm::accumulate(s1, e1, init$ as byte*, mul) as double* @;
	if(res != std::lib::atof("87178291200")) fail();

	init = 0;
	res = algorithm::accumulate(s1, e1, init$ as byte*, custom) as double* @;
	if(res != 7) fail();
	iterator::delete_iterator(s1), iterator::delete_iterator(e1);
	
	op = 0;
	s1 = vector::begin(v), e1 = vector::end(v);
	init = 0;
	res = algorithm::accumulate(s1, e1, init$ as byte*, custom) as double* @;
	if(res != -7) fail();
	iterator::delete_iterator(s1), iterator::delete_iterator(e1);

	vector::delete_vector(v);
	success();
}
