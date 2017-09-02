import <"std/hooks">
import <"std/syscall">

namespace std {
	namespace hooks {

		access type exit_data | qed;
		access bool | qed_init;

		func void sp_quick_exit(int status) {
			if(qed_init)
				for(int i = qed.sz - 1; i >= 0; i--)
					qed.exit_fn_list[i]();
			std::syscall::exit(status);
		}
	}
}
