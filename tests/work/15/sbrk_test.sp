import <"std/syscall">

func int main() {
	using namespace std::syscall;
	char* c = direct_sbrk(1) as char*;
	direct_write(1, (c@ = 1 + '0', c), 1);
	direct_write(1, (c@ = 4 + '0', c), 1);
	direct_write(1, (c@ = '\n', c), 1);
	if(direct_sbrk(-1) as int == -1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	return 0;
}
