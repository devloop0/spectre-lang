import "../test.sp"

struct s {
	int i, j;
}

func type s f() {
	struct s { int k, l; }
	type s s1;
	type ::s s2;
	s1.k = 1, s1.l = 2;
	s2.i = 3, s2.j = 4;
	if (s1.k != 1 || s1.l != 2) fail();
	return s2;
}

func int main() {
	struct s { int m, n; }
	auto ss = f();
	type s _s; _s.m = 5, _s.n = 6;
	if (ss.i != 3 || ss.j != 4) fail();
	if (_s.m != 5 || _s.n != 6) fail();
	success();
}
