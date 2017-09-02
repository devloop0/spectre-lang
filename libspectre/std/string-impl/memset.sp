import <"std/string">

namespace std {
	namespace string {

		func byte* memset(byte* dst, const byte val, unsigned int bytes) {
			byte* orig = dst;
			while(bytes-- as bool)
				dst@ = val, dst = dst[1]$;
			return orig;
		}
	}
}
