import <"stdx/vector">
import <"std/lib">

namespace stdx {
	namespace vector_impl {

		func type vector* new_vector(unsigned int u) {
			using std::lib::malloc;
	
			type vector* v = malloc(sizeof{type vector}) as type vector*;
			v->unit = u;
			v->capacity = INIT_CAP;
			v->size = 0;
			v->data = malloc(v->unit * v->capacity);
			return v;
		}
	}
}
