import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {
	
		func void quick_exit(int status) {
			return std::hooks::sp_quick_exit(status);
		}
	}
}
