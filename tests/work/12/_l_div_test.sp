import <"std/syscall">
import <"std/lib">

using std::syscall::direct_write;
using std::syscall::exit;
using std::lib::div;
using std::lib::ldiv;
using std::lib::ldiv_t;
using std::lib::div_t;

func int main() {
	type div_t result;
	result = div(38, 5);
	if(result.quot != 7 || result.rem != 3) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	type ldiv_t lresult = ldiv(1000000_L, 132_l);
	if(lresult.quot != 7575. || lresult.rem != 100 ||
		lresult.quot != 7575) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
