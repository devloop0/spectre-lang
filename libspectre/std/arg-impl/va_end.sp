import <"std/arg">
import <"std/lib">

namespace std {
	namespace arg {
	
		func void va_end(type va_list* v) {
			std::lib::free(v as byte*);
		}
	}
}
