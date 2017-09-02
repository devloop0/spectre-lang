import <"std/syscall">

func void mat_trans(double** dst, float** src, unsigned int n) {
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			dst[i][j] = src[j][i];
}

func int main() {
	float** src = [ [1_f, 2_f, 3_f],
			[4_f, 5_f, 6_f],
			[7_f, 8_f, 9_f] ];
	double** dst = stk double*(3);
	dst@ = stk double(3),
		dst[1] = stk double(3),
		dst[2] = stk double(3);
	double** r_t = [ [1., 4., 7.],
			[2., 5., 8.],
			[3., 6., 9.] ];
	mat_trans(dst, src, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			if(dst[i][j] != r_t[i][j]) {
				std::syscall::direct_write(1, "N\n", 2);
				return 1;
			}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
