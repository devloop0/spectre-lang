import <"std/lib">
import <"std/ctype">
import <"std/syscall">

using std::syscall::direct_write;
using std::ctype::isblank;
using std::lib::_Exit;

func void fail() {
	direct_write(1, "N\n", 2);
	_Exit(1);
}

func int main() {
	if(!isblank(' ') || isblank(23) || isblank('a') || !isblank('\t') || isblank('\n')) fail();
	direct_write(1, "Y\n", 2);
}
