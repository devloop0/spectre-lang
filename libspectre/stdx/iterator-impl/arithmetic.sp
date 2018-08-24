import <"stdx/iterator">

namespace stdx {
	namespace iterator_impl {

		func type iterator* add(const type iterator* it1, int off) {
			return it1->arith_fn(it1, off);
		}

		func type iterator* advance(type iterator* it1, int off) {
			type iterator* temp = it1->arith_fn(it1, off);
			it1->data_copy_fn(it1, temp);
			delete_iterator(temp);
			return it1;
		}

		func type iterator* next(type iterator* it1) {
			return advance(it1, 1);
		}

		func type iterator* sub(const type iterator* it1, int off) {
			return it1->arith_fn(it1, -off);
		}

		func type iterator* back(type iterator* it1, int off) {
			return advance(it1, -off);
		}

		func type iterator* prev(type iterator* it1) {
			return advance(it1, -1);
		}

		func int iterator_sub(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return 0;
			else if(it1->iter_arith_fn != it2->iter_arith_fn) return 0;
			else if(it1->data_structure != it2->data_structure) return 0;
			
			return it1->iter_arith_fn(it1, it2);
		}
	}
}
