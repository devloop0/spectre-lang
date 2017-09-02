import <"std/lib">
import <"std/string">

namespace std {
	namespace lib {

		func[static] void swap(byte* a, byte* b, unsigned int size) {
			while(size-- > 0) {
				byte tmp = a@;
				a@ = b@;
				b@ = tmp;
				a = a[1]$, b = b[1]$;
			}
		}

		func void qsort(byte* arr, unsigned int len, unsigned int sz, fn int(const byte*,const byte*) cmp) {
			if(len < 2) return;
			byte* pivot = stk byte(2048);
			std::string::memcpy(pivot, arr[(len / 2) * sz]$, sz);
			int i = 0, j = (len - 1) * sz;
			for(; ; i += sz, j -= sz) {
				while(cmp(arr[i]$, pivot) < 0) i += sz;
				while(cmp(arr[j]$, pivot) > 0) j -= sz;
				
				if(i >= j) break;

				swap(arr[i]$, arr[j]$, sz);
			}
			qsort(arr, i / sz, sz, cmp);
			qsort(arr[i]$, len - i / sz, sz, cmp);
		}
	}
}
