import "../test.sp"

constexpr int x = 2;

func void test_x() {
	if(x != 2) fail();
}

func int main() {
	constexpr int x = 3;
	{
		constexpr int x = 4;
		if(::x != 2) fail();
		if(x != 4) fail();
	}
	if(::x != 2) fail();
	if(x != 3) fail();
	test_x();
	success();
}
