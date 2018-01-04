import <"std/math">
import <"std/string">

namespace std {
	namespace math {

		func float infinity() {
			int i = 0x7F80'0000;
			float ret;
			std::string::memcpy(ret$ as byte*, i$ as byte*, sizeof(ret));
			return ret;
		}
	}
}
