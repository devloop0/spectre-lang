import <"stdx/vector">
import <"stdx/iterator">
import <"std/lib">

namespace stdx {
	namespace vector_impl {

		func type iterator::iterator* begin(const type vector* v) {
			using std::lib::malloc;

			type vector_iterator_data* data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			data->pos = 0;
			return iterator::new_iterator(data as byte*, false, v as byte*,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}

		func type iterator::iterator* rbegin(const type vector* v) {
			using std::lib::malloc;

			type vector_iterator_data* data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			data->pos = size(v) - 1;
			return iterator::new_iterator(data as byte*, true, v as byte*,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}

		func type iterator::iterator* end(const type vector* v) {
			using std::lib::malloc;

			type vector_iterator_data* data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			data->pos = size(v);
			return iterator::new_iterator(data as byte*, false, v as byte*,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}

		func type iterator::iterator* rend(const type vector* v) {
			using std::lib::malloc;

			type vector_iterator_data* data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			data->pos = -1;
			return iterator::new_iterator(data as byte*, true, v as byte*,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}
	}
}
