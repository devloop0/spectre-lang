import <"std/syscall">

struct four_quarters {
	unsigned short b0, b1, b2, b3;
}

type four_quarters x;
int a, b;

func void f(type four_quarters j) {
	b = j.b2;
	a = j.b3;
}

func int main() {
	type four_quarters x;
	x.b0  = x.b1 = x.b2 = 0;
	x.b3 = 38;
	f(x);
	if(a != 38) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
