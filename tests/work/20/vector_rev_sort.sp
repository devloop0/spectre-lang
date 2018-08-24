import <"stdx/vector">
import <"std/lib">

import "../test.sp"

func int int_rev_cmp(const byte* a, const byte* b) {
	return a as const int* @ > b as const int* @ ? -1 :
		a as const int* @ == b as const int* @ ? 0 : 1;
}

func int main() {
	using namespace stdx::vector;
	using std::lib::qsort;

	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);

	for(int i = 1; i <= 100; i++)
		vector::append(v, i$ as byte*);

	for(int i = 1; i <= 100; i++)
		if(vector::at(v, i - 1) as int* @ != i) fail();

	if(vector::size(v) != 100) fail();

	qsort(vector::data(v), vector::size(v), vector::unit(v), int_rev_cmp);

	for(int i = 0; i < 100; i++)
		if(vector::at(v, i) as int* @ != 100 - i) fail();

	success();
}
