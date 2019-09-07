import <"std/ctype">
import <"std/lib">
import <"std/syscall">


func int main() {
	using std::lib::_Exit;
	using std::syscall::direct_write;
	using std::ctype::isalnum;
	char* str = "c3po...", orig = str;
	while(isalnum(str@)) str = str[1]$;
	if(str as unsigned int - orig as unsigned int != 4) {
		direct_write(1, "N\n", 2);
		_Exit(1);
	}
	direct_write(1, "Y\n", 2);
}
