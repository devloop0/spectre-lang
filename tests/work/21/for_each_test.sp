import <"stdx/vector">
import <"stdx/iterator">
import <"stdx/algorithm">

import "../test.sp"

using namespace stdx::iterator;
using namespace stdx::vector;
using namespace stdx::algorithm;

func void x2(byte* b) {
	b as int* @ *= 2;
}

func int main() {
	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);
	
	for(int i = 0; i < 10; i++) vector::append(v, i$ as byte*);
	for(int i = 0; i < 10; i++) if(vector::at(v, i) as int* @ != i) fail();

	type iterator::iterator* s1 = vector::begin(v), e1 = vector::end(v);
	algorithm::for_each(s1, e1, x2);
	iterator::delete_iterator(s1), iterator::delete_iterator(e1);

	for(int i = 0; i < 10; i++) if(vector::at(v, i) as int* @ != i * 2) fail();

	success();
}
