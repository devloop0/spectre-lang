import <"stdx/iterator">

namespace stdx {
	namespace iterator_impl {

		func byte* deref(const type iterator* it1) {
			return it1->deref_fn(it1);
		}
	}
}
