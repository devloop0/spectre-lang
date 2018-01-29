import <"stdx/vector">
import <"std/lib">

namespace stdx {
	namespace vector_impl {

		func byte* at(const type vector* v, unsigned int idx) {
			using std::lib::NULL;
			if(idx < 0 || idx >= v->size) return NULL;
			return v->data[idx * v->unit]$;
		}
	}
}
