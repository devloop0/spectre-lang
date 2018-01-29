import <"stdx/vector">
import <"std/lib">

namespace stdx {
	namespace vector_impl {

		func void delete_vector(type vector* v) {
			using std::lib::free;
			
			free(v->data);
			free(v as byte*);
		}
	}
}
