import <"std/syscall">, <"std/aliases">

func int main(int argc, char** argv) {
	using namespace std::aliases;

	auto nl = '\n', sp = ' ';
	for(int i = 1; i < argc; i++) {
		char* s = argv[i];
		while(s@ as bool) {
			sys::direct_write(1, s, 1);
			if(s[1] as bool)
				sys::direct_write(1, sp$, 1);
			s = s[1]$;
		}
		sys::direct_write(1, nl$, 1);
	}

	return 0;
}
