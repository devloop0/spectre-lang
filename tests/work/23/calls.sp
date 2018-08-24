import "../test.sp"

func int f() { return 1; }
func int g() { return 2; }
func int h() { return 3; }

func int call(unsigned int n, fn int()* ops) {
	int total = 0;
	for(unsigned int i = 0; i < n; i++) {
		if(i % 2 == 0) total += ops[i]();
		else total -= ops[i]();
	}
	return total;
}

func int main() {
	if(call(3, [f, g, h]) != 2) fail();
	if(call(3, [g, h, f]) != 0) fail();
	if(call(3, [h, f, g]) != 4) fail();
	if(call(3, [f, h, g]) != 0) fail();
	if(call(3, [h, g, f]) != 2) fail();
	if(call(3, [g, f, h]) != 4) fail();
	
	success();
}
