import <"std/core">
import <"std/io">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_signed_short(const signed short s __rfuncarg__) {
				return std::core::asm_syscall(1);
			}

			func[static] const bool print_const_unsigned_short(const unsigned short u __rfuncarg__) {
				return std::core::asm_syscall(1);
			}
		}

		func const bool print_short(const signed short s __rfuncarg__) {
			return _detail::print_const_signed_short(s);
		}

		func const bool print_ushort(const unsigned short u __rfuncarg__) {
			return _detail::print_const_unsigned_short(u);
		}

		func const bool print_short_ln(const signed short s __rfuncarg__) {
			return _detail::print_const_signed_short(s) && print_ln();
		}
	
		func const bool print_ushort_ln(const unsigned short u __rfuncarg__) {
			return _detail::print_const_unsigned_short(u) && print_ln();
		}
	}
}
