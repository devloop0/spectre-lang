import <"std/io">
import <"std/ctype">
import <"std/aliases">
import "../test.sp"

using namespace std::aliases;

func int main() {
	char* tmp1 = ({
		char* arr = [ 'R', 'w', 'l', 'l', 'o', 0 as char ];
		arr[1] = 'e';
		char* ptr = arr;
		arr;
	});
	char* tmp2 = ({
		char* arr = [ 'F', 'g', 'r', 'l', 'd', 0 as char ];
		arr[1] = 'o';
		char* ptr = arr;
		arr;
	});
	tmp2@ = ctype::tolower((tmp1@ = 'H') + 15);
	io::printf("%s", tmp1);
	io::printf(" ");
	io::printf("%s!\n", tmp2);
	success();
}
