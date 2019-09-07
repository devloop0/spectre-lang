import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {

		func type ldiv_t ldiv(long num, long denom) {
			type ldiv_t d;
			d.quot = std::hooks::idiv(num, denom);
			d.rem = std::hooks::imod(num, denom);
			return d;
		}
	}
}
