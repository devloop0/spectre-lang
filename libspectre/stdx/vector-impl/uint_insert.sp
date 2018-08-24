import <"stdx/vector">
import <"std/lib">
import <"std/string">

namespace stdx {
	namespace vector_impl {
	
		func int uint_insert(type vector* v, unsigned int idx, const byte* data) {
			using std::lib::realloc;
			using std::string::memmove;
			using std::string::memcpy;

			if(idx > v->size) return -1;
			else if(idx == v->size) return append(v, data);

			if(v->capacity == 0 || !v->data as bool) {
				v->capacity = INIT_CAP;
				v->data = realloc(v->data, v->capacity * v->unit);
				if(!v->data as bool) return -1;
			}
			if(v->size >= v->capacity) {
				v->data = realloc(v->data, v->unit * (v->capacity *= 2));
				if(!v->data as bool) return -1;
			}
			unsigned int byte_offset = v->unit * idx;
			unsigned int bytes_to_move = (v->size - idx) * v->unit;
			memmove(v->data[byte_offset + v->unit]$, v->data[byte_offset]$, bytes_to_move);
			memcpy(v->data[byte_offset]$, data, v->unit);
			v->size++;
			return 0;
		}
	}
}
