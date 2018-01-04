import <"std/lib">
import <"std/string">

namespace std {
	namespace lib {
	
		func byte* calloc(const unsigned int n, const unsigned int sz) {
			using std::string::memset;

			byte* ret = malloc(n * sz);
			return memset(ret, 0, n * sz);
		}
	}
}
