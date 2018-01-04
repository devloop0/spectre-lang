import <"std/syscall">
import <"std/string">

func int main(int argc, char** argv) {
	using std::string::strlen;
	using std::syscall::direct_write;

	if(argc == 1) return 0;

	char nl = '\n', sp = ' ';
	for(int i = 1; i < argc; i++) {
		direct_write(1, argv[i], strlen(argv[i]));
		direct_write(1, sp$, 1);
	}
	direct_write(1, nl$, 1);
}
