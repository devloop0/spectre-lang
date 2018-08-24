import <"stdx/algorithm">
import <"stdx/iterator">

namespace stdx {
	namespace algorithm_impl {

		func unsigned long count(type iterator::iterator* s1, type iterator::iterator* e1, fn bool(byte*) pred) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			unsigned long ret = 0;
			for(; iterator::neq(s, e); iterator::next(s))
				if(pred(iterator::deref(s))) ret++;
			iterator::delete_iterator(s), iterator::delete_iterator(e);
			return ret;
		}
	}
}
