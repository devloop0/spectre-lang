import <"std/syscall">
import <"std/hooks">

namespace std {
	namespace hooks {
		
		access type exit_data | ed;
		access bool | ed_init;

		func void sp_exit(int status) {
			if(ed_init)
				for(int i = ed.sz - 1; i >= 0; i--)
					ed.exit_fn_list[i]();
			std::syscall::exit(status);
		}
	}
}
