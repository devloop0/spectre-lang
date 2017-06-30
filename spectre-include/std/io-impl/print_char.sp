import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_signed_char(const signed char s) {
				return std::core::asm_syscall(11);
			}

			func[static] const bool print_const_unsigned_char(const unsigned char u) {
				return std::core::asm_syscall(11);
			}
		}

		func const bool print_char(const signed char s) {
			return _detail::print_const_signed_char(s);
		}

		func const bool print_uchar(const unsigned char u) {
			return _detail::print_const_unsigned_char(u);
		}

		func const bool print_char_ln(const signed char s) {
			return _detail::print_const_signed_char(s) && print_ln();
		}

		func const bool print_uchar_ln(const unsigned char u) {
			return _detail::print_const_unsigned_char(u) && print_ln();
		}
	}
}
