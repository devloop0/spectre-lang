import <"stdx/iterator">
import <"std/lib">

namespace stdx {
	namespace iterator_impl {

		func type iterator* new_iterator(byte* dt, bool r, byte* ds,
			fn type iterator*(const type iterator*,int) a,
			fn int(const type iterator*,const type iterator*) ia,
			fn byte*(const type iterator*) d,
			fn bool(const type iterator*,const type iterator*,unsigned int) c,
			fn void(type iterator*) f,
			fn void(type iterator*,const type iterator*) dc,
			fn type iterator*(const type iterator*) ic) {
			using std::lib::malloc;

			type iterator* ret = malloc(sizeof{type iterator}) as type iterator*;
			ret->data_structure = ds;
			ret->data = dt;
			ret->reverse = r;
			ret->arith_fn = a;
			ret->iter_arith_fn = ia;
			ret->deref_fn = d;
			ret->cmp_fn = c;
			ret->free_fn = f;
			ret->data_copy_fn = dc;
			ret->iter_clone_fn = ic;
			return ret;
		}
	}
}
