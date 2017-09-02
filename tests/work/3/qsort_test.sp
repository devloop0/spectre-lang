import <"std/syscall">
import <"std/string">
import <"std/lib">

func int int_cmp(const byte* a, const byte* b) {
	return (a as int* @ - b as int* @);
}

func int main() {
	char* arr = stk char(21);
	arr[1] = arr[3] = arr[5] = arr[7] = arr[9] = arr[11] = arr[13] = arr[15] = arr[17] = ' ';
	arr[19] = '\n', arr[20] = 0;
	int* my_list = [7, 3, 2, 8, 4, 5, 0, 6, 1, 9];
	for(int i = 0; i < 20; i += 2) arr[i] = my_list[i / 2] + '0';
	std::syscall::direct_write(1, arr, std::string::strlen(arr));
	std::lib::qsort(my_list as byte*, 10, sizeof{int}, int_cmp);
	for(int i = 0; i < 20; i += 2) arr[i] = my_list[i / 2] + '0';
	std::syscall::direct_write(1, arr, std::string::strlen(arr));
	return 0;
}
