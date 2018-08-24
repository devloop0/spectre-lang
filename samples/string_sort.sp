import <"stdx/string">
import <"stdx/vector">
import <"std/lib">
import <"std/syscall">

func int stdx_string_cmp(const byte* a, const byte* b) {
	using namespace stdx::string;

	return string::lt(a as const type string::string** @, b as const type string::string** @) ?
		-1 : string::eq(a as const type string::string** @, b as const type string::string** @) ? 0 : 1;
}

func int main(int argc, char** argv) {
	using std::syscall::direct_write;
	using std::lib::qsort;
	using namespace stdx::vector;
	using namespace stdx::string;

	if(argc == 1) return 0;

	type vector::vector* vec = vector::new_vector(sizeof{type string::string*});
	for(int i = 1; i < argc; i++) {
		type string::string* temp = string::new_string(argv[i]);
		vector::append(vec, temp$ as byte*);
	}

	qsort(vector::data(vec), vector::size(vec), vector::unit(vec), stdx_string_cmp);

	for(int i = 0; i < vector::size(vec); i++) {
		type string::string* temp = vector::at(vec, i) as type string::string** @;
		string::concatch(temp, '\n');
		direct_write(1, string::data(temp), string::size(temp));
		string::delete_string(temp);
	}

	vector::delete_vector(vec);

	return 0;
}
