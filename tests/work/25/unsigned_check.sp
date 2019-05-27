import <"std/syscall">

func int main() {
	namespace sys = std::syscall;

	unsigned int u = 0x82'34'56'78; // == 2'184'468'088;
	while (u as bool) {
		char c = u % 10 + '0';
		sys::direct_write(1, c$, 1);
		u /= 10;
	}
	char nl = '\n'; sys::direct_write(1, nl$, 1);
	return 0;
}
