import <"std/arg">
import <"std/lib">

namespace std {
	namespace arg {

		func type va_list* va_start(byte* last_arg, unsigned int sz) {
			type va_list* v = std::lib::malloc(sizeof{type va_list}) as type va_list*;
			v->start = last_arg[sz]$;
			return v;
		}
	}
}
