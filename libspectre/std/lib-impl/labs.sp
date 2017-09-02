import <"std/lib">

namespace std {
	namespace lib {
	
		func long labs(long n) {
			long mask = n >> 31, cmask = ~mask;
			return mask & -n | cmask & n;
		}
	}
}
