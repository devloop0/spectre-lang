import <"std/machine">

namespace std {
	namespace machine {

		func const bool middle_endian() {
			return !little_endian() && !big_endian();
		}
	}
}
