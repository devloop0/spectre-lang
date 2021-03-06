import <"std/syscall">
import <"std/string">

import "../test.sp"

using namespace std::syscall;

func int main() {
	type stat statbuf;
	direct_stat("../20", statbuf$);
	if(!S_ISDIR(statbuf.st_mode)) fail();
	direct_stat("../test.sp", statbuf$);
	if(!S_ISREG(statbuf.st_mode)) fail();

	success();
}
