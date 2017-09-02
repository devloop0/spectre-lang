import <"std/hooks">

namespace std {
	namespace hooks {

		type exit_data qed;
		bool qed_init = false;

		func int sp_at_quick_exit(fn void() f) {
			if(!qed_init) {
				qed.exit_fn_list = resv fn void()(32);
				qed.sz = 0;
				qed_init = true;
			}
			if(qed.sz >= 32) return -1;
			qed.exit_fn_list[qed.sz++] = f;
			return 0;
		}
	}
}
