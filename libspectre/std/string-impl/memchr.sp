import <"std/string">

namespace std {
	namespace string {
	
		func byte* memchr(const byte* src, const byte val, unsigned int bytes) {
			byte* siter = src as byte*;
			while(bytes-- as bool) {
				if(siter@ == val) return siter;
				siter = siter[1]$;
			}
			return NULL as byte*;
		}
	}
}
