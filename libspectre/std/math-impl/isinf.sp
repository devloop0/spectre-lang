import <"std/math">

namespace std {
	namespace math {

		func bool isinf(double arg) {
			return arg == infinity() || arg == -infinity();
		}
	}
}
