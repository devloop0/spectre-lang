import <"std/lib">
import <"std/hooks">

namespace std {
	namespace lib {
		
		func void exit(int status) {
			return std::hooks::sp_exit(status);
		}
	}
}
