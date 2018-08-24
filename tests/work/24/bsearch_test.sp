import <"std/lib">
import "../test.sp"

func int cmp(const byte* a, const byte* b) {
	const int* ia = a as const int*,
		ib = b as const int*;
	if(ia@ < ib@)
		return -1;
	else if(ia@ > ib@)
		return 1;
	else
		return 0;
}

func void bsearch_checker(int* arr, unsigned int limit, unsigned int low, unsigned int high) {
	using std::lib::NULL;
	using std::lib::bsearch;

	for(unsigned int i = 0; i <= limit; i++) {
		const byte* res = bsearch(i$ as byte*, arr as byte*, high - low + 1, sizeof{int}, cmp);
		if(i < low) {
			if (res != NULL) fail();
		}
		else if(low <= i && i <= high) {
			if (res == NULL || res as int* @ != i) fail();
		}
		else {
			if (res != NULL) fail();
		}
	}
}

func int main() {
	int* arr1 = [ 3, 4, 5, 6, 7 ], arr2 = [ 4, 5, 6, 7 ];
	bsearch_checker(arr1, 10, 3, 7);
	bsearch_checker(arr2, 10, 4, 7);
	success();
}
