import <"stdx/vector">

import "../test.sp"

func int main() {
	using namespace stdx::vector;

	type vector::vector* v1 = vector::new_vector_capacity(sizeof{int}, 1),
		v2 = vector::new_vector_capacity(sizeof{int}, 1);
	for(int i = 0; i < 100; i++) if(vector::append(v1, i$ as byte*) as bool) fail();
	for(int i = 99; i >= 0; i--) if(vector::append(v2, i$ as byte*) as bool) fail();

	if(vector::size(v1) != vector::size(v2)) fail();
	if(vector::size(v1) != 100) fail();

	for(int i = 0; i < vector::size(v1); i++)
		if(vector::at(v1, i) as int* @
			+ vector::at(v2, i) as int* @
			!= 99) fail();

	vector::delete_vector(v1);
	vector::delete_vector(v2);

	success();
}
