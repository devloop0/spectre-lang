import <"std/syscall">

namespace std {
	namespace syscall {

		byte* current_brk = 0 as byte*;
		
		func int direct_brk(const byte* data) {
			byte* ret = current_brk = raw_brk(data);
			if(ret as unsigned int < data as unsigned int)
				return -1;
			else
				return 0;
		}
	}
}
