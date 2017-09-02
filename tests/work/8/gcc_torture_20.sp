import <"std/syscall">

func int imax(int x, int y) { return x > y ? x : y; }
func int imin(int x, int y) { return x < y ? x : y; }
func unsigned int umax(unsigned int x, unsigned int y) { return x > y ? x : y; }
func unsigned int umin(unsigned int x, unsigned int y) { return x < y ? x : y; }

func int main() {
	unsigned int u; int i = -1;
	u = umax((i > 0 ? i : 0) as unsigned int, 1);
	if(u != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	int tmp = i > 0 ? i : 0;
	u = (i > 0 ? i : 0) < i as unsigned int ? (i > 0 ? i : 0) : i as unsigned int;
	if(u != 0) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
