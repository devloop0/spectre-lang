import <"stdx/vector">
import <"std/string">

import "../test.sp"

using namespace stdx::vector;
using namespace std::string;

func int main() {
	int* arr = [ 0, 1, 2, 8, 10, 17, 25 ]; int len = 7;

	int* f = [ 3, 4, 5, 6, 7 ], s = [ 9 ], t = [ 11, 12, 13, 14, 15, 16 ], fr = [18, 19, 20, 21, 22, 23, 24 ];


	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);
	
	for(int i = 0; i < len; i++) if(vector::append(v, arr[i]$ as byte*) != 0) fail();

	for(int i = 0; i < 5; i++) if(vector::uint_insert(v, 3 + i, f[i]$ as byte*) != 0) fail();
	for(int i = 0; i < 1; i++) if(vector::uint_insert(v, 9 + i, s[i]$ as byte*) != 0) fail();
	for(int i = 0; i < 6; i++) if(vector::uint_insert(v, 11 + i, t[i]$ as byte*) != 0) fail();
	for(int i = 0; i < 7; i++) if(vector::uint_insert(v, 18 + i, fr[i]$ as byte*) != 0) fail();

	for(int i = 0; i < 25; i++) if(vector::at(v, i) as int* @ != i) fail();

	success();
}
