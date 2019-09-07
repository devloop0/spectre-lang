import <"std/math">
import <"std/machine">
import <"std/lib">

namespace std {
	namespace math {

		func float fabsf(float arg) {
			union uu {
				int x;
				float f;
			}
			type uu u;

			u.f = arg;
			u.x &= 0x7f'ff'ff'ff;
			return u.f;
		}
	}
}
