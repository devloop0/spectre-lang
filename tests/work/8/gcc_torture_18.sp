import <"std/syscall">

int count = 0;

func int foo1() {
	count++;
	return 0;
}

func int foo2() {
	count++;
	return 0;
}

func int main() {
	if(((foo1() == 1) as int & (foo2() == 1) as int) as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(count != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
