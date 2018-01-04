import <"std/syscall">

namespace std {
	namespace syscall {

		access byte* | current_brk;

		func byte* curr_brk() {
			if(!current_brk as bool) direct_brk(0 as byte*);
			return current_brk;
		}
	}
}
