import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_signed_long(const signed long s __rfuncarg__) {
				return std::core::asm_syscall(1);
			}

			func[static] const bool print_const_unsigned_long(const unsigned long u __rfuncarg__) {
				return std::core::asm_syscall(36);
			}
		}

		func const bool print_long(const signed long s __rfuncarg__) {
			return _detail::print_const_signed_long(s);
		}
	
		func const bool print_ulong(const unsigned long u __rfuncarg__) {
			return _detail::print_const_unsigned_long(u);
		}

		func const bool print_long_ln(const signed long s __rfuncarg__) {
			return _detail::print_const_signed_long(s) && print_ln();
		}

		func const bool print_ulong_ln(const unsigned long u __rfuncarg__) {
			return _detail::print_const_unsigned_long(u) && print_ln();
		}
	}
}
