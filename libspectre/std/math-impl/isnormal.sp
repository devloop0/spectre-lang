import <"std/math">

namespace std {
	namespace math {

		func bool isnormal(double arg) {
			byte* arr = arg$ as byte*;
			byte exp_1 = arr@ & 0x7f,
				exp_2 = arr[1] & 0xe0;
			return !(exp_1 == 0 && exp_2 == 0) &&
				!(exp_1 == 0x7f && exp_2 == 0xe0);
		}
	}
}
