import <"std/lib">

namespace std {
	namespace lib {

		func byte* bsearch(const byte* key, const byte* arr, unsigned int len, unsigned int sz, fn int(const byte*, const byte*) cmp) {
			unsigned int low = 0, high = len - 1;
			while (low <= high) {
				unsigned int mid = low + (high - low) / 2;
				const byte* mid_elem = arr[mid * sz]$;
				int cmp_res = cmp(mid_elem, key);
				if(low == high) {
					if(cmp_res == 0) return mid_elem as byte*;
					else return NULL;
				}
				else if(cmp_res == 0)
					return mid_elem as byte*;
				else if(cmp_res < 0)
					low = mid + 1;
				else
					high = mid;
			}
			return NULL;
		}
	}
}
