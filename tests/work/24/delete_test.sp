import "../test.sp"

func int f(int a) { return a + 2; }

func int main(int argc, char** argv) {
	if(f(2) != 4) fail();

	byte* b = new byte(1);
	delete b;

	byte** b2 = new byte*(23);
	delete b2;

	if(f(4) != 6) fail();

	int i = 0;
	for (; i < 20; i++) {
		int*** j = new int**(1212);
		delete j;

		if(f(8) != 10) fail();

		if (i == 15) {
			f(2);
			break;
		}
	}

	if (i != 15) fail();

	float** g = new float*(2);
	delete g;

	if(f(11) != 13) fail();

	short* s = new short(2);
	delete s;

	char** c = new char*(2);
	c[0] = new char(2);
	c[1] = new char(3);
	delete c[0];
	delete c[1];
	delete c;

	if(f(15) != 17) fail();

	double* d = new double(2);
	delete d;

	success();
}
