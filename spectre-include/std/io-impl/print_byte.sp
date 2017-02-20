import <"std/io">
import <"std/core">

namespace std {
	namespace io {
	
		namespace _detail {

			func[static] const bool print_const_signed_byte(const signed byte s __rfuncarg__) {
				const signed int i = s;
				return print_int(i);
			}

			func[static] const bool print_const_unsigned_byte(const unsigned byte u __rfuncarg__) {
				return print_uint(u & 0xFF);
			}
		}

		func const bool print_byte(const signed byte s __rfuncarg__) {
			return _detail::print_const_signed_byte(s);
		}

		func const bool print_ubyte(const unsigned byte u __rfuncarg__) {
			return _detail::print_const_unsigned_byte(u);
		}

		func const bool print_byte_ln(const signed byte s __rfuncarg__) {
			return _detail::print_const_signed_byte(s) && print_ln();
		}

		func const bool print_ubyte_ln(const unsigned byte u __rfuncarg__) {
			return _detail::print_const_unsigned_byte(u) && print_ln();
		}
	}
}
