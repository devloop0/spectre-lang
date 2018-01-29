import <"std/syscall">
import <"std/ctype">
import <"std/string">
import <"std/lib">

using std::syscall::direct_write;
using std::lib::_Exit;
using std::string::strlen;
using std::ctype::isxdigit;

func void fail() {
	direct_write(1, "N\n", 2);
	_Exit(1);
}

func int main() {
	int res = 0;
	char* test = "023fFaBcg3";
	for(int i = 0; i < strlen(test); i++) res += isxdigit(test[i]) as int;
	if(res != strlen(test) - 1) fail();
	direct_write(1, "Y\n", 2);
}
