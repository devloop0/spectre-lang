import <"std/syscall">
import <"std/string">

struct A {
	unsigned short a1;
	unsigned long a2;
}

struct B {
	int b1, b2, b3, b4, b5;
}

struct C {
	type B* c1;
	int c2, c3;
}

func[static] int foo(int x) {
	return x < 0 ? -x : x;
}

func int bar(type C* x, type A* y) {
	int a = x->c3;
	const int b = y->a1 >> 9;
	const unsigned long c = y->a2;
	int d = a;
	unsigned long e, f;

	f = foo(c - x->c1[d].b4);
	do {
		if(d <= 0)
			d = x->c2;
		d--;
		
		e = foo(c-x->c1[d].b4);
		if(e < f)
			a = d;
	} while(d != x->c3);
	x->c1[a].b4 = c + b;
	return a;
}

func int main() {
	type A a;
	type C b;
	int c;
	
	a.a1 = 512;
	a.a2 = 4242;
	std::string::memset(b$ as byte*, 0, sizeof(b));

	b.c1 = stk type B(1);
	b.c1[0].b3 = 424242;
	b.c2 = 1;
	c = bar(b$, a$);

	if(b.c1[0].b4 != 4243) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
