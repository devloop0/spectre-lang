import <"stdx/vector">

import "../test.sp"

func int main() {
	using namespace stdx::vector;

	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);

	for(int i = 0; i < 10; i++)
		if(vector::append(v, i$ as byte*) as bool) fail();

	if(vector::size(v) != 10) fail();

	for(int i = 100'000; i < 100'010; i++)
		vector::at(v, i - 100'000) as int* @ = i;
	
	if(vector::size(v) != 10) fail();

	for(int i = 0; i < 10; i++) 
		if(vector::at(v, i) as int* @ != i + 100'000) fail();

	vector::delete_vector(v);

	success();
}
