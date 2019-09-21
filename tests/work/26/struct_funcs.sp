import "../test.sp"

struct s {
	fn int(type s,int,float) a;
	fn type s(type s) copy;
	int b, c;
}

func int temp(type s ss, int a, float b) { return a + b * ss.b - ss.c; }
func type s temp2(type s ss) { return ss; }
func type s create() { type s ss; ss.a = temp; ss.copy = temp2; ss.b = (ss.c = 3) - 1; return ss; }
func int main() { 
	if (create().copy(create()).copy(create()).a(create(), 4, 5) != 11) fail();
	success();
}
