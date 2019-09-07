import <"std/arg">
import <"std/string">

namespace std {
	namespace arg {

		func byte* va_arg(type va_list* v, byte* dest, unsigned int sz, unsigned int elem_alignment) {
			unsigned int curr_offs = v->start as unsigned int;
			if (curr_offs % elem_alignment != 0) {
				unsigned int from_alignment = curr_offs % elem_alignment,
					to_next_alignment = elem_alignment - (curr_offs % elem_alignment);
				v->start = v->start[to_next_alignment]$;
			}
			std::string::memcpy(dest, v->start, sz);
			v->start = v->start[sz]$;
			return dest;
		}
	}
}
