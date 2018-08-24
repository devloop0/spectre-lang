import "../test.sp"

func double f() {
	{
		static auto i = 0;
		return i++;
	}
}

func int main() {
	for(auto i = 0_f; i < 10; i++)
		if(f() != i) fail();
	success();
}
