import <"std/lib">
import <"std/syscall">
import <"std/string">

func int main(int argc, char** argv) {
	using std::lib::atoi;
	using std::string::memset;
	using std::syscall::direct_write;

	if(argc != 2) return 0;
	char* num = argv[1];
	int orig = atoi(num), done = orig + 2;
	if(orig < 0) return 0;

	char* arr = stk char(10);
	char nl = '\n';
	memset(arr as byte*, 0, 10);
	int iter = 0;
	while(iter < 10 && done as bool) {
		arr[iter] = (done % 10) + '0';
		done /= 10, iter++;
	}

	for(int i = iter - 1; i >= 0; i--)
		direct_write(1, arr[i]$, 1);
	direct_write(1, nl$, 1);
	return 0;
}
