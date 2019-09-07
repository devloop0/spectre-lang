import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {

		func type div_t div(int num, int denom) {
			type div_t d;
			d.quot = std::hooks::idiv(num, denom);
			d.rem = std::hooks::imod(num, denom);
			return d;
		}
	}
}
