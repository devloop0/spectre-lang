import <"std/syscall">

struct t { int i, j; int* k; int** l; }

type t s;

func type t* f() { s.i = 3; s.j = 5; s.k = resv int(2);
		s.k@ = 7, s.k[1] = 9; s.l = s.k$; return s$; }
func type t* g() { s.i = 2; s.j = 4; s.k = resv int(2);
		s.k@ = 6, s.k[1] = 8; s.l = s.k$; return s$; }

func int main() {
	if((((((((((((((false ? (((f))) : (((g))))())))->l)))@)))[1]))) != 8) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
