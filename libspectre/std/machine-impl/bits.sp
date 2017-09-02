import <"std/machine">

namespace std {
	namespace machine {
		
		func const unsigned int bits() {
			return sizeof{int*} << 3;
		}
	}
}
