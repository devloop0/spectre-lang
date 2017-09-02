import <"std/syscall">
import <"std/string">
import <"std/lib">

func int cmpoddeven(const byte* xp, const byte* yp) {
	int x = xp as int* @,
		y = yp as int* @;
	if(x == y) return 0;
	if(x % 2 == y % 2)
		return x < y ? -1 : 1;
	if(x % 2 == 1)
		return -1;
	return 1;
}

func int main() {
	int* array = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
	char* to_print = stk char(21);
	to_print[1] = to_print[3] = to_print[5] = to_print[7] =
		to_print[9] = to_print[11] = to_print[13] = to_print[15] =
		to_print[17] = ' ';
	to_print[19] = '\n', to_print[20] = 0;
	for(int i = 0; i < 10; i++) to_print[i * 2] = array[i] + '0';
	std::syscall::direct_write(1, to_print, std::string::strlen(to_print));
	std::lib::qsort(array as byte*, 10, sizeof{int}, cmpoddeven);
	for(int i = 0; i < 10; i++) to_print[i * 2] = array[i] + '0';
	std::syscall::direct_write(1, to_print, std::string::strlen(to_print));
	return 0;
}
