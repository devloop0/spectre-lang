import <"stdx/iterator">

namespace stdx {
	namespace iterator_impl {

		func type iterator* copy(const type iterator* s) {
			return s->iter_clone_fn(s);
		}
	}
}
