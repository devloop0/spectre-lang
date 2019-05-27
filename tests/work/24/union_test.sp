import "../test.sp"

union u {
	unsigned int i;
	float f;
}

func int main() {
	type u uu;
	uu.f = 12.53;
	if (uu.i != 0x41487ae1) fail();
	success();
}
