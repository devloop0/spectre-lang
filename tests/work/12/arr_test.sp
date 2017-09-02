import <"std/syscall">

using namespace std::syscall;

int** arr = [ [1, 2, 3], [4, 5, 6], [7, 8, 9] ];
int* arr2 = [2, 4, 6, 8];

func int main() {
	if(arr[0][0] != 1 || arr[0][1] != 2 || arr[0][2] != 3 ||
		arr[2][2] != 9 || arr[2][1] != 8 || arr[2][0] != 7 ||
		arr[1][1] != 5 || arr[1][2] != 6 || arr[1][0] != 4) {
		direct_write(1, "N\n", 2);
		return 1;
	}
	if(arr2[2] + 0_f != 6. || arr2[1] != 4._f || arr2[0] + 0. != 2 ||
		arr2[3] != 8) {
		direct_write(1, "N\n", 2);
		exit(1);
	}
	int* arr3 = [1, 3, 5, 7, 9];
	for(int i = 0; i < 5; i++)
		if(arr3[i] != 2*i + 1) {
			direct_write(1, "N\n", 2);
			exit(1);
		}
	int** arr4 = [[0, 2, 4, 6, 8], arr3];
	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 5; j++)
			if(arr4[i][j] != 2*j + i) {
				direct_write(1, "N\n", 2);
				exit(1);
			}
	direct_write(1, "Y\n", 2);
	return 0;
}
