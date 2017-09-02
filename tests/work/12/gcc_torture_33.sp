import <"std/syscall">

using std::syscall::direct_write;
using std::syscall::exit;

func int f2();
func int f3();
func void f1();

func void ff(int fname, int part, int nparts) {
	if(fname as bool) {
		if(nparts as bool) f1();
	}
	else
		fname = 2;

	while(f3() as bool) {
		if(nparts as bool) {
			if(f2() as bool) {
				f1();
				nparts = part;
				if(f3() as bool) f1();
				f1();
				break;
			}
		}
	}
	if(nparts as bool)
		f1();
	return;
}

func int main() {
	ff(0, 1, 0);
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}

func int f3() { static int x = 0; x = (!x as bool) as int; return x; }
func void f1() { direct_write(1, "N\n", 2); exit(1); }
func int f2() { direct_write(1, "N\n", 2); exit(1); }
