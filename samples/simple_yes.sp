import <"std/syscall">
import <"std/string">
import <"std/aliases">

func int main(int argc, char** argv) {
	using namespace std::aliases;

	auto nl = '\n', y = 'y', sp = ' ';
	if (argc < 2) {
		while (true) {
			sys::direct_write(1, y$, 1);
			sys::direct_write(1, nl$, 1);
		}
	}
	else {
		while (true) {
			for (int i = 1; i < argc; i++) {
				sys::direct_write(1, argv[i], str::strlen(argv[i]));
				sys::direct_write(1, sp$, 1);
			}
			sys::direct_write(1, nl$, 1);
		}
	}
	return 0;
}
