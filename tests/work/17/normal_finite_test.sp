import <"std/math">

import "../test.sp"

func int main() {
	using namespace std::math;

	if(isfinite(nan())) fail();
	if(isfinite(infinity())) fail();
	if(!isfinite(0.0)) fail();
	if(!isfinite(1.0)) fail();
	if(!isfinite(-1.0)) fail();

	if(isnormal(nan())) fail();
	if(isnormal(infinity())) fail();
	if(isnormal(0.)) fail();
	if(isnormal(-0.0)) fail();
	if(!isnormal(1.2) || !isnormal(-1.2)) fail();

	success();
}
