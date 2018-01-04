import <"std/lib">
import <"std/syscall">

func void fail() {
	std::syscall::direct_write(1, "N\n", 2);
	std::syscall::exit(1);
}

func void success() {
	std::syscall::direct_write(1, "Y\n", 2);
	std::syscall::exit(0);
}

func int main() {
	using std::lib::atol;

	if(atol("	  -123asdkfjn") != -123) fail();
	if(atol("0") != 0) fail();
	if(atol("asdkjfn") != 0) fail();
	if(atol("+12345") != 12345) fail();
	if(atol("29823") != 29823) fail();
	success();
}
