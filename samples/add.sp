import <"std/lib">
import <"std/syscall">
import <"std/string">

func int main(int argc, char** argv) {
	using std::lib::atoi;
	using std::lib::malloc;
	using std::lib::free;
	using std::lib::abs;
	using std::string::memset;
	using std::syscall::direct_write;

	if(argc == 1) return 0;

	int sum = 0, iter = 0;
	bool neg = false;
	char nl = '\n', d = '-';

	for(int i = 1; i < argc; i++) {
		int temp = atoi(argv[i]);
		sum += temp;
	}
	if(sum == 0) {
		char* temp = "0\n";
		direct_write(1, temp, 2);
		return 0;
	}
	if(sum < 0) neg = true;
	sum = abs(sum);

	char* c = malloc(10) as char*;
	memset(c as byte*, 0, 10);
	if(neg) direct_write(1, d$, 1);
	for(; iter < 10 && sum as bool; iter++, sum /= 10)
		c[iter] = (sum % 10) + '0';
	for(int i = iter - 1; i >= 0; i--)
		direct_write(1, c[i]$, 1);
	free(c as byte*);

	direct_write(1, nl$, 1);
}
