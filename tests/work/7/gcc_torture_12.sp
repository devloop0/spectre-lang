import <"std/syscall">
import <"std/string">

struct c {
	int j, k, l;
	int* c;
}

func int sub1(int i, int j) {
	type c c;
	c.c = new int(i + 2);
	int* x = stk int(10), y = stk int(10);
	if(j == 2) {
		std::string::memcpy(x as byte*, y as byte*, 10 * sizeof{int});
		return sizeof(c);
	}
	else
		return sizeof(c) * 3;
}

struct d {
	short d, e, f, g;
}

func int main() {
	if(sub1(20, 3) != sizeof{type d} * 3) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
