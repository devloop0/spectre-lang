import <"std/io">
import <"std/core">

namespace std {
	namespace io {

		namespace _detail {
	
			func[static] const bool print_const_float(const float f __rfuncarg__) {
				return std::core::asm_syscall(2);
			}
		}

		func const bool print_float(const float f __rfuncarg__) {
			return _detail::print_const_float(f);
		}

		func const bool print_float_ln(const float f __rfuncarg__) {
			return _detail::print_const_float(f) && print_ln();
		}
	}
}
