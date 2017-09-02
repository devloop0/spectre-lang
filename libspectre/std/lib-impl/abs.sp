import <"std/lib">

namespace std {
	namespace lib {
		
		func int abs(int n) {
			int mask = n >> 31, cmask = ~mask;
			return mask & -n | cmask & n;
		}
	}
}
