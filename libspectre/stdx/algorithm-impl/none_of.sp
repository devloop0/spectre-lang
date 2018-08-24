import <"stdx/algorithm">
import <"stdx/iterator">

namespace stdx {
	namespace algorithm_impl {

		func bool none_of(type iterator::iterator* s1, const type iterator::iterator* e1, fn bool(byte*) pred) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			for(; iterator::neq(s, e); iterator::next(s))
				if(pred(iterator::deref(s))) {
					iterator::delete_iterator(s), iterator::delete_iterator(e);
					return false;
				}
			iterator::delete_iterator(s), iterator::delete_iterator(e);
			return true;
		}
	}
}
