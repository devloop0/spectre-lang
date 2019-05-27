import <"std/arg">
import <"std/string">

namespace std {
	namespace arg {

		func byte* va_arg(type va_list* v, byte* dest, unsigned int sz) {
			std::string::memcpy(dest, v->start, sz);
			v->start = v->start[align(sz)]$;
			return dest;
		}
	}
}
