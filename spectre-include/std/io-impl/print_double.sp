import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_double(const double d __rfuncarg__) {
				return std::core::asm_syscall(3);
			}
		}

		func const bool print_double(const double d __rfuncarg__) {
			return _detail::print_const_double(d);
		}

		func const bool print_double_ln(const double d __rfuncarg__) {
			return _detail::print_const_double(d) && print_ln();
		}
	}
}
