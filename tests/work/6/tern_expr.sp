import <"std/syscall">

func int main() {
	float f = 1, o = 2;
	(true ? (((f))) : (((o)))) = 2;
	if(f != o) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	int i = 1, j = 2;
	(true ? i : j) = 2;
	if(i != j) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	float r = 3;
	((((true ? ((((false ? (((f))) : (((o))))))) : (((r))))))) = 7;
	if(f != 2 || o != 7 || r != 3) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	float* fp = f$, op = o$, rp = r$;
	(false ? fp : (true ? op : rp))@ = 4;
	if(f != 2 || o != 4 || r != 3) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
}
