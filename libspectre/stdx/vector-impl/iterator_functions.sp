import <"stdx/vector">
import <"std/lib">

namespace stdx {
	namespace vector_impl {

		func type iterator::iterator* vector_iterator_int_arith(const type iterator::iterator* it1, int off) {
			using std::lib::malloc;

			type vector_iterator_data* it_data = it1->data as type vector_iterator_data*;
			type vector* v = it1->data_structure as type vector*;
			int new_pos;
			if(it1->reverse) {
				off = -off;
				if(it_data->pos + off >= size(v) as int)
					new_pos = size(v) - 1;
				else if(it_data->pos + off < -1)
					new_pos = -1;
				else
					new_pos = it_data->pos + off;
			}
			else {
				if(it_data->pos + off >= size(v) as int)
					new_pos = size(v);
				else if(it_data->pos + off < 0)
					new_pos = 0;
				else
					new_pos = it_data->pos + off;
			}
		
			type vector_iterator_data* new_it_data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			new_it_data->pos = new_pos;
			return iterator::new_iterator(new_it_data as byte*, it1->reverse, v as byte*,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}

		func int vector_iterator_iter_arith(const type iterator::iterator* it1, const type iterator::iterator* it2) {
			return (it1->data as type vector_iterator_data*)->pos - (it2->data as type vector_iterator_data*)->pos;
		}

		func byte* vector_iterator_deref(const type iterator::iterator* it1) {
			type vector* v = it1->data_structure as type vector*;
			return v->data[(it1->data as type vector_iterator_data*)->pos * unit(v)]$;
		}

		func bool vector_iterator_cmp(const type iterator::iterator* it1, const type iterator::iterator* it2, unsigned int d) {
			int pos1 = (it1->data as type vector_iterator_data*)->pos,
				pos2 = (it2->data as type vector_iterator_data*)->pos;
			switch(d) {
				case iterator::cmp_dispatch::EQ: return pos1 == pos2;
				case iterator::cmp_dispatch::NEQ: return pos1 != pos2;
				case iterator::cmp_dispatch::LT: return it1->reverse ? pos1 > pos2 : pos1 < pos2;
				case iterator::cmp_dispatch::LTE: return it1->reverse ? pos1 >= pos2 : pos1 <= pos2;
				case iterator::cmp_dispatch::GT: return it1->reverse ? pos1 < pos2 : pos1 > pos2;
				case iterator::cmp_dispatch::GTE: return it1->reverse ? pos1 <= pos2 : pos1 >= pos2;
			}
			return false;
		}

		func void vector_iterator_free(type iterator::iterator* it) {
			using std::lib::free;
			
			free(it->data as byte*);
		}

		func void vector_iterator_data_copy(type iterator::iterator* it1, const type iterator::iterator* it2) {
			it1->data as type vector_iterator_data* ->pos = it2->data as type vector_iterator_data* -> pos;
		}

		func type iterator::iterator* vector_iterator_clone(const type iterator::iterator* it1) {
			using std::lib::malloc;
		
			type vector_iterator_data* data = malloc(sizeof{type vector_iterator_data}) as type vector_iterator_data*;
			data->pos = it1->data as type vector_iterator_data* ->pos;
			return iterator::new_iterator(data as byte*, it1->reverse, it1->data_structure,
				vector_iterator_int_arith, vector_iterator_iter_arith,
				vector_iterator_deref, vector_iterator_cmp, vector_iterator_free, vector_iterator_data_copy,
				vector_iterator_clone);
		}
	}
}
