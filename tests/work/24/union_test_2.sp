import "../test.sp"

union u {
	unsigned int i;
	float f;
}

func void f(type u uu) {
	if (uu.i != 0x41490000 || uu.f != 12.5625)
		fail();
}

func void g(type u* uu) {
	if (uu->i != 0x41490000 || uu->f != 12.5625)
		fail();
	uu->f = 123.5;
}

func int main() {
	type u uu;
	uu.f = 12.5625;
	f(uu);
	g(uu$);
	if (uu.f != 123.5 || uu.i != 0x42f70000)
		fail();
	success();
}
