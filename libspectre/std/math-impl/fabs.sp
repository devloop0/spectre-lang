import <"std/lib">
import <"std/math">
import <"std/machine">

namespace std {
	namespace math {

		func double fabs(double arg) {
			byte* b = arg$ as byte*;
			if (std::machine::little_endian()) b[sizeof{double} - 1] &= 0x7f;
			else if (std::machine::big_endian()) b@ &= 0x7f;
			else std::lib::abort();
			return arg;
		}
	}
}
