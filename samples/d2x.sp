import <"std/string">
import <"std/lib">
import <"std/syscall">

func void d2x(int i, char* c) {
	using std::string::strcat;
	using std::lib::abs;
	using std::string::memset;

	bool neg = i < 0;
	i = abs(i);

	if(i == 0) {
		strcat(c, "0x0");
		return;
	}

	char* rev = stk char(20);
	int iter =  0;
	memset(rev as byte*, 0, 20);
	while(iter < 20 && i as bool) {
		int to_check = i % 16;
		if(to_check <= 9)
			rev[iter] = '0' + to_check;
		else
			rev[iter] = (to_check - 10) + 'A';
		iter++, i /= 16;
	}

	if(neg) strcat(c, "-");
	strcat(c, "0x");
	for(int i = iter - 1, j = 2 + neg as int; i >= 0; i--, j++)
		c[j] = rev[i];
}

func int main(int argc, char** argv) {
	using std::string::memset;
	using std::string::strlen;
	using std::lib::malloc;
	using std::lib::free;
	using std::syscall::direct_write;
	using std::lib::atoi;

	if(argc == 1) return 0;

	char* str = malloc(20) as char*;
	char nl = '\n';
	memset(str as byte*, 0, 20);
	for(int i = 1; i < argc; i++) {
		d2x(atoi(argv[i]), str);
		direct_write(1, str, strlen(str));
		memset(str as byte*, 0, 20);
		direct_write(1, nl$, 1);
	}
}
