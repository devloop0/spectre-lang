import <"stdx/algorithm">
import <"stdx/iterator">

namespace stdx {
	namespace algorithm_impl {

		func void for_each(type iterator::iterator* s1, const type iterator::iterator* e1, fn void(byte*) pred) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			for(; iterator::neq(s, e); iterator::next(s))
				pred(iterator::deref(s));
			iterator::delete_iterator(s), iterator::delete_iterator(e);
		}
	}
}
