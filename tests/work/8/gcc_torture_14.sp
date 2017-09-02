import <"std/syscall">

int* pwarn;

func void bla() {
	if(!pwarn@ as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		std::syscall::exit(1);
	}
	std::syscall::direct_write(1, "Y\n", 2);
	std::syscall::exit(0);
}

func int main() {
	int warn;

	pwarn = warn$;
	warn = 1;
	bla();
}
