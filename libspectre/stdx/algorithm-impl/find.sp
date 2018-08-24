import <"stdx/algorithm">
import <"stdx/iterator">

namespace stdx {
	namespace algorithm_impl {

		func type iterator::iterator* find_if(type iterator::iterator* s1, type iterator::iterator* e1, fn bool(byte*) pred) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			for(; iterator::neq(s, e); iterator::next(s))
				if(pred(iterator::deref(s))) break;
			iterator::delete_iterator(e);
			return s;
		}

		func type iterator::iterator* find_if_not(type iterator::iterator* s1, type iterator::iterator* e1, fn bool(byte*) pred) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			for(; iterator::neq(s, e); iterator::next(s))
				if(!pred(iterator::deref(s))) break;
			iterator::delete_iterator(e);
			return s;
		}
	}
}
