import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_signed_int(const signed int s) {
				return std::core::asm_syscall(1);
			}

			func[static] const bool print_const_unsigned_int(const unsigned int u) {
				return std::core::asm_syscall(36);
			}
		}

		func const bool print_int(const signed int s) {
			return _detail::print_const_signed_int(s);
		}

		func const bool print_uint(const unsigned int u) {
			return _detail::print_const_unsigned_int(u);
		}

		func const bool print_int_ln(const signed int s) {
			return _detail::print_const_signed_int(s) && print_ln();
		}

		func const bool print_uint_ln(const unsigned int u) {
			return _detail::print_const_unsigned_int(u) && print_ln();
		}
	}
}
