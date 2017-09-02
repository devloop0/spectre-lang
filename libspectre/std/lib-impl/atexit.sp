import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {

		func int atexit(fn void() f) {
			return std::hooks::sp_atexit(f);
		}
	}
}
