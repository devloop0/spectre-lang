import <"std/syscall">
import <"std/string">

func int main() {
	using std::string::strlen;
	int* arr = [2, 3, 4];
	char* c = stk char(8);
	for(int i = 0; i < 3; i++) c[i*2] = arr[i] + '0', c[i*2 + 1] = ' ';
	c[6] = '\n', c[7] = 0;
	std::syscall::direct_write(1, c, strlen(c));
}
