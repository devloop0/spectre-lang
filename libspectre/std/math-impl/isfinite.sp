import <"std/math">

namespace std {
	namespace math {

		func bool isfinite(double arg) {
			return !isnan(arg) && !isinf(arg);
		}
	}
}
