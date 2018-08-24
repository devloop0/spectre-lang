import <"stdx/iterator">

namespace stdx {
	namespace iterator_impl {

		func bool lt(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::LT);
		}

		func bool gt(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::GT);
		}

		func bool eq(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::EQ);
		}

		func bool lte(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::LTE);
		}

		func bool gte(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::GTE);
		}

		func bool neq(const type iterator* it1, const type iterator* it2) {
			if(it1->reverse != it2->reverse) return false;
			else if(it1->cmp_fn != it2->cmp_fn) return false;
			else if(it1->data_structure != it2->data_structure) return false;

			return it1->cmp_fn(it1, it2, cmp_dispatch::NEQ);
		}
	}
}
