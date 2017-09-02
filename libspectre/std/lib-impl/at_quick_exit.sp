import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {
	
		func int at_quick_exit(fn void() f) {
			return std::hooks::sp_at_quick_exit(f);
		}
	}
}
