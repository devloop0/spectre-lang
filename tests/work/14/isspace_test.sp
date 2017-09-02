import <"std/lib">
import <"std/syscall">
import <"std/ctype">
import <"std/string">

using std::lib::_Exit;
using std::syscall::direct_write;
using std::ctype::isspace;
using std::string::NULL;
using std::string::strlen;

char* tok = 0 as char*;

func char* split(char* str) {
	if(str != NULL && tok == NULL) tok = str;
	char* orig = tok, iter = tok;
	if(!orig@ as bool) {
		tok = NULL;
		return NULL;
	}
	while(iter@ as bool && !isspace(iter@)) iter = iter[1]$;
	if(iter@ as bool) {
		iter@ = 0;
		tok = iter[1]$;
	}
	else tok = iter;
	return orig;
}

func int main() {
	char* str = "This\tis C3PO :^).", iter;
	while((iter = split(str)) as bool) {
		direct_write(1, iter, strlen(iter));
		direct_write(1, "-\n", 2);
	}
}
