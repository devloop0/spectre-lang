import <"std/string">
import <"std/lib">
import <"std/syscall">
import <"std/ctype">

func void x2d(char* src, char* dest) {
	using std::ctype::tolower;
	using std::ctype::isxdigit;
	using std::ctype::isdigit;
	using std::string::strcpy;
	using std::string::strcat;
	using std::string::memset;
	using std::lib::abs;

	int num = 0;
	bool neg = false;
	if(src@ == '-') neg = true, src = src[1]$;
	
	if(src@ != '0' && tolower(src[1]) != 'x') {
		strcpy(dest, "0");
		return;
	}
	src = src[2]$;

	while(src@ as bool) {
		if(!isxdigit(src@)) {
			strcpy(dest, "0");
			return;
		}
		num *= 16;
		if(isdigit(src@)) num += src@ - '0';
		else num += (tolower(src@) - 'a') + 10;
		src = src[1]$;
	}

	char* rev = stk char(20);
	memset(rev as byte*, 0, 20);
	int iter = 0;
	if(num == 0) {
		strcpy(dest, "0");
		return;
	}

	while(iter < 20 && num as bool) {
		rev[iter++] = (num % 10) + '0';
		num /= 10;
	}

	if(neg) strcat(dest, "-");
	for(int i = iter - 1, j = neg as int; i >= 0; i--, j++)
		dest[j] = rev[i];
}

func int main(int argc, char** argv) {
	using std::string::memset;
	using std::string::strlen;
	using std::lib::atoi;
	using std::lib::malloc;
	using std::lib::free;
	using std::syscall::direct_write;

	if(argc == 1) return 0;

	char nl = '\n';
	char* str = malloc(20) as char*;
	memset(str as byte*, 0, 20);
	for(int i = 1; i < argc; i++) {
		x2d(argv[i], str);
		direct_write(1, str, strlen(str));
		direct_write(1, nl$, 1);
		memset(str as byte*, 0, 20);
	}
}
