import <"stdx/vector">

func int main() {
	using namespace stdx::vector;
	
	type vector::vector* v = vector::new_vector(sizeof{int}),
		v2 = vector::new_vector_capacity(sizeof{int}, 20);
	
	vector::delete_vector(v);
	vector::delete_vector(v2);
}
