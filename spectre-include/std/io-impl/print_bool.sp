include <"std/io">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_bool(const bool b __rfuncarg__) {
				return print_ubyte(b ? 0b1 : 0b0);
			}
		}

		func const bool print_bool(const bool b __rfuncarg__) {
			return _detail::print_const_bool(b);
		}

		func const bool print_bool_ln(const bool b __rfuncarg__) {
			return _detail::print_const_bool(b) && print_ln();
		}
	}
}
