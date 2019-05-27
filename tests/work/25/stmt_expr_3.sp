import <"std/io">
import "../test.sp"

func void temp(int* i) { i@ += 94; }

func int main() {
	int j;
	({ if (false) j = 1; else j = 2; temp(j$); if (j != 96) j++; else --j; });
	if (j != 95) fail();
	success();
}
