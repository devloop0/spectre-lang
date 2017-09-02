import <"std/string">

namespace std {
	namespace string {

		func[static] byte* memmove_base(byte* dst, const byte* src,
			unsigned int bytes, bool fwd) {
			byte* orig = dst, siter = src as byte*;
			while(bytes-- as bool)
				dst@ = siter@, dst = dst[fwd ? 1 : -1]$,
					siter = siter[fwd ? 1 : -1]$;
			return orig;
		}
		
		func byte* memmove(byte* dst, const byte* src, unsigned int bytes) {
			byte* orig = dst;
			if(dst as unsigned int > src as unsigned int) {
				if(src as unsigned int + bytes > dst as unsigned int)
					memmove_base(dst[bytes - 1]$, src[bytes - 1]$, bytes, false);
				else
					memmove_base(dst, src, bytes, true);
			}
			else if(dst == src) dst = src as byte*;
			else
				memmove_base(dst, src, bytes, true);
			return orig;
		}
	}
}
