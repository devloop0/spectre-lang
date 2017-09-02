import <"std/machine">

namespace std {
	namespace machine {

		func const bool big_endian() {
			int x = 0xAA'BB'CC'DD;
			return x$ as char* @ == 0xAA;
		}
	}
}
