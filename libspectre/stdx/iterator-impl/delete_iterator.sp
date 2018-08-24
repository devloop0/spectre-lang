import <"stdx/iterator">
import <"std/lib">

namespace stdx {
	namespace iterator_impl {
		
		func void delete_iterator(type iterator* it) {
			using std::lib::free;

			fn void(type iterator*) free_fn = it->free_fn;
			free_fn(it);
			free(it as byte*);
		}
	}
}
