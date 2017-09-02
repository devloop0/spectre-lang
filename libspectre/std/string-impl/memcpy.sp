import <"std/string">

namespace std {
	namespace string {

		func byte* memcpy(byte* dst, const byte* src, unsigned int bytes) {
			byte* s = src as byte*, orig = dst;
			while(bytes-- as bool)
				dst@ = s@, dst = dst[1]$, s = s[1]$;
			return orig;
		}
	}
}
