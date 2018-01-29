import <"stdx/vector">
import <"std/lib">
import <"std/string">

namespace stdx {
	namespace vector_impl {

		func void clear(type vector* v) {
			using std::lib::realloc;
			using std::string::memset;
			
			v->size = 0;
			if(v->capacity > INIT_CAP) {
				v->capacity = INIT_CAP;
				v->data = realloc(v->data, v->capacity * v->unit);
				if(!v->data as bool) return;
			}
			memset(v->data, 0, v->capacity * v->unit);
		}
	}
}
