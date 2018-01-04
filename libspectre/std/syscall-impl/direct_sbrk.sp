import <"std/syscall">

namespace std {
	namespace syscall {

		access byte* | current_brk;

		func byte* direct_sbrk(const int inc) {
			if(!current_brk as bool)
				if(direct_brk(0 as byte*) < 0)
					return (-1) as byte*;

			if(inc == 0) return current_brk;

			byte* old_brk = current_brk;
			if(direct_brk(old_brk[inc]$) < 0)
				return (-1) as byte*;

			return old_brk;
		}
	}
}
