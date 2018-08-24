import <"stdx/vector">
import <"stdx/iterator">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace stdx::iterator;
	using namespace stdx::vector;
	using std::string::strlen;

	int* data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], data_iter = data;

	type vector::vector* v = vector::new_vector_capacity(sizeof{int}, 1);
	for(int i = 0; i < 10; i++) vector::append(v, i$ as byte*);

	int iters = 0;
	type iterator::iterator* it = vector::begin(v), end = vector::end(v);
	for(; iterator::neq(it, end); iterator::next(it), data_iter = data_iter[1]$, iters++)
		if(iterator::deref(it) as int* @ != data_iter@) fail();

	if(iters != 10) fail();

	data_iter = data[9]$;
	type iterator::iterator* rit = vector::rbegin(v), rend = vector::rend(v);
	iters = 0;
	for(; iterator::neq(rit, rend); iterator::next(rit), data_iter = data_iter[-1]$, iters++)
		if(iterator::deref(rit) as int* @ != data_iter@) break;

	if(iters != 10) fail();

	iterator::delete_iterator(it);
	iterator::delete_iterator(end);
	iterator::delete_iterator(rit);
	iterator::delete_iterator(rend);
	vector::delete_vector(v);
	success();
}
