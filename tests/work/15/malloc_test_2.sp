import <"std/lib">

func int main() {
	using std::lib::malloc;
	using std::lib::free;

	int* i = malloc(100) as int*;
	free(i as byte*);
	int* j = malloc(100) as int*;
	free(j as byte*);
	int* k = malloc(50) as int*, l = malloc(50) as int*;
	free(k as byte*), free(l as byte*);
}
