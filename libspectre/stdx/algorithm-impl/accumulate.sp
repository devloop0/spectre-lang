import <"stdx/algorithm">
import <"stdx/iterator">

namespace stdx {
	namespace algorithm_impl {
		
		
		func byte* accumulate(type iterator::iterator* s1, const type iterator::iterator* e1, byte* init,
			fn byte*(byte*,byte*) op) {
			type iterator::iterator* s = iterator::copy(s1), e = iterator::copy(e1);
			for(; iterator::neq(s, e); iterator::next(s))
				init = op(init, iterator::deref(s));
			iterator::delete_iterator(s), iterator::delete_iterator(e);
			return init;
		}
	}
}
