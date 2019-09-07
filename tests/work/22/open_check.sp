import <"std/syscall">

import "../test.sp"

func int main() {
	using namespace std::syscall;
	
	int fd = direct_open("err", O_RDONLY);
	int fd2 = direct_open("open_check.sp", O_RDONLY);

	if(fd != -1) fail();
	direct_close(fd);
	if(fd2 < 0) fail();
	direct_close(fd2);

	success();
}
