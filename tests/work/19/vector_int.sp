import <"stdx/vector">

import "../test.sp"

func int main() {
	using namespace stdx::vector;

	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);
	for(int i = 99; i >= 0; i--) if(vector::append(v, i$ as byte*) as bool) fail();
	if(vector::size(v) != 100) fail();

	for(int i = 0; i < 100; i++) if(vector::at(v, 99 - i) as int* @ != i) fail();

	vector::clear(v);
	if(vector::size(v) != 0) fail();

	vector::delete_vector(v);

	success();
}
