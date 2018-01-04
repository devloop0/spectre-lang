import <"std/math">
import <"std/lib">

import "../test.sp"

# from http://en.cppreference.com/w/c/numeric/math/fabs
func double num_int(double a, double b, fn double(double) f, unsigned int n) {
	using std::math::fabs;

	if(a == b) return 0.;
	if(n == 0) n = 1;
	double h = (b - a) / n, sum = 0;
	for(unsigned int k = 0; k < n; k++)
		sum += h * fabs(f(a + k * h));
	return sum;
}

func double f_x(double x) { return x; }

func int main() {
	using namespace std::math;

	if(fabsf(+3._f) != 3_f) fail();
	if(fabsf(-12.3_f) != 12.3_F) fail();
	if(fabsf(-infinity()) != infinity()) fail();
	if(!isnan(fabsf(-nan()))) fail();
	if(fabs(+2.) != 2) fail();
	if(fabs(-123.443) != 1.23443e2) fail();
	if(fabs(-infinity()) != infinity()) fail();
	if(!isnan(fabs(-nan()))) fail();

	if(num_int(-2, 2, f_x, 100000) != 4.0000000000000027) fail();
	# ^ precision, sigh
	success();
}
