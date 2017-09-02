import <"std/syscall">

func void mat_trans(double* dst, double* src, unsigned int n) {
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			dst[i*n + j] = src[j*n + i];
}

func int main() {
	double* s = [ 1., 2., 3.,
		    4., 5., 6.,
		    7., 8., 9. ];
	double* d = stk double(9);
	float* r_t = [ 1_f, 4_f, 7_f,
		    2_f, 5_f, 8_f,
		    3._f, 6_f, 9_f ];
	mat_trans(d, s, 3);
	for(int i = 0; i < 9; i++)
		if(d[i] != r_t[i]) {
			std::syscall::direct_write(1, "N\n", 2);
			return 1;
		}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
