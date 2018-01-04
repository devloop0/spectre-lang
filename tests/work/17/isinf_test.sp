import <"std/math">

import "../test.sp"

func int main() {
	using namespace std::math;

	double d_inf = infinity();
	float f_inf = infinity();

	double d_nan = nan();
	float f_nan = nan();

	if(isinf(d_nan) || isinf(f_nan)) fail();
	if(!isinf(d_inf) || !isinf(f_inf)) fail();
	if(!isinf(-d_inf) || !isinf(-f_inf)) fail();
	if(isinf(0.0) || !isinf(infinity())) fail();
	if(isinf(1.0) || isinf(-1)) fail();
	success();
}
