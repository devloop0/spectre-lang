import <"std/syscall">
import <"std/lib">

func int main() {
	using namespace std::syscall;
	byte* temp = curr_brk();
	int res1 = direct_brk((-1) as byte*);
	int res2 = direct_brk(temp[10]$);
	if(res1 != -1 || res2 != 0) {
		direct_write(1, "N\n", 2);
		return res1;
	}
	direct_write(1, "Y\n", 2);
	return 0;
}
