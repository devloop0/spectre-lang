import <"std/string">

namespace std {
	namespace string {

		func int memcmp(const byte* ptr1, const byte* ptr2, unsigned int count) {
			byte* p1 = ptr1 as byte*, p2 = ptr2 as byte*;
			while(count-- as bool) {
				if(p1@ != p2@) break;
				p1 = p1[1]$, p2 = p2[2]$;
			}
			return count == 0 ? count : p1@ - p2@;
		}
	}
}
