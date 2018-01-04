import "../test.sp"
import <"std/math">
import <"std/lib">


func int main() {
	using std::lib::atof;
	using std::math::infinity;
	using ::std::math::isnan;

	if(atof("0.1") != 0.1) fail();
	if(atof("0.0001") != 1e-4) fail();
	if(atof("123.546") != 1.23546e+2) fail();
	if(atof("1.2e1") != 12) fail();
	if(atof("1.2e+5") != 120000) fail();
	if(atof("1.2e-5") != 0.000012) fail();
	if(atof("15e16e") != 1.5e17) fail();
	if(atof("15e+16") != 1.5e17) fail();
	if(atof("  	-12367.2e9") != -1.23672e13) fail();
	if(atof("-0x1afp-2") != -1.0775e+2) fail();
	if(atof("-iNf") != -infinity()) fail();
	if(!isnan(atof("NaN(123)"))) fail();
	if(atof("0.0") != 0) fail();
	if(atof("skdajf") != 0) fail();
	if(atof("0x.4p-2") != atof("0x1p-4")) fail();
	if(atof(".001e-5") * atof("0.001e3") != 1e-8) fail();
	if(atof("  -0.000000000123adlkfjgndf") != -1.23e-10) fail();
	success();
}
