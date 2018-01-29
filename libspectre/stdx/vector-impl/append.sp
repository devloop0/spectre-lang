import <"stdx/vector">
import <"std/lib">
import <"std/string">

namespace stdx {
	namespace vector_impl {

		func int append(type vector* v, const byte* b) {
			using std::string::memcpy;
			using std::lib::realloc;

			if(v->capacity == 0 || !v->data as bool) {
				v->capacity = INIT_CAP;
				v->data = realloc(v->data, v->unit * v->capacity);
			}
			if(v->size >= v->capacity) {
				v->data = realloc(v->data, v->unit * (v->capacity *= 2));
				if(!v->data as bool) return -1;
			}
			memcpy(v->data[v->size++ * v->unit]$, b, v->unit);
			return 0;
		}
	}
}
