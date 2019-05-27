import <"std/arg">
import <"std/lib">

namespace std {
	namespace arg {

		func type va_list* va_start(unsigned int aligned_pre, byte* last_arg) {
			type va_list* v = std::lib::malloc(sizeof{type va_list}) as type va_list*;
			v->start = last_arg[aligned_pre + 16 + aligned_pre]$ as byte*;
		}
	}
}
