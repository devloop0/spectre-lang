import <"std/syscall">

func void fail() {
	std::syscall::direct_write(1, "N\n", 2);
	std::syscall::exit(1);
}

func void success() {
	std::syscall::direct_write(1, "Y\n", 2);
	std::syscall::exit(0);
}
