import <"std/math">
import <"std/machine">
import <"std/lib">

namespace std {
	namespace math {

		func double fabs(double arg) {
			using std::lib::abort;
			using std::machine::big_endian;
			using std::machine::little_endian;

			byte* b = arg$ as byte*;
			if(big_endian()) b@ &= 0x7f;
			else if(little_endian()) b[sizeof{double} - 1] &= 0x7f;
			else abort();
			
			return arg;
		}
	}
}
