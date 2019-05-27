import <"std/arg">

namespace std {
	namespace arg {

		func unsigned int align(unsigned int sz) {
			return !(sz & 7) as bool ? sz : ((sz >> 3) + 1) << 3;
		}
	}
}
