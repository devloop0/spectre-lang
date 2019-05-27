import <"std/alloca">
import <"std/lib">

namespace std {
	namespace lib {

		func void freea(const byte* data) {
			free(data);
		}
	}
}
