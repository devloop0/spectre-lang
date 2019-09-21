import "../test.sp"

func int m() { return 2; }
func fn int() n() { return m; }
func int main() {
	if (n()() != 2) fail();
	success();
}
