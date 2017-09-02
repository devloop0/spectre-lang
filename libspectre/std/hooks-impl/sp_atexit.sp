import <"std/hooks">

namespace std {
	namespace hooks {
	
		type exit_data ed;
		bool ed_init = false;

		func int sp_atexit(fn void() f) {
			if(!ed_init) {
				ed.exit_fn_list = resv fn void()(32);
				ed.sz = 0;
				ed_init = true;
			}
			if(ed.sz >= 32) return -1;
			ed.exit_fn_list[ed.sz++] = f;
			return 0;
		}
	}
}
