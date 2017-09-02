import <"std/machine">

namespace std {
	namespace machine {

		func const bool little_endian() {
			int x = 0xAA'BB'CC'DD;
			return x$ as char* @ == 0xDD;
		}
	}
}
