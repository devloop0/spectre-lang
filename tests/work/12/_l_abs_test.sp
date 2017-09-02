import <"std/syscall">
import <"std/lib">

using std::syscall::direct_write;
using std::syscall::exit;
using std::lib::abs;
using std::lib::labs;

func int main() {
	int n = abs(23), m = abs(-11), l = abs(0);
	if(n != 23 || m != 11 || l != 0) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	long ln = labs(65537_L), lm = labs(-100000_L), ll = labs(0_L);
	if(ln != 65537_L || lm != 100000_L || ll != 0_L) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
