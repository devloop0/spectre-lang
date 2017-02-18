import <"std/core">
import <"std/io">

namespace std {
	namespace io {

		namespace _detail {

			func[static] const bool print_const_char_pointer(const char* str __rfuncarg__) {
				return std::core::asm_syscall(4);
			}
		}

		func const bool print_string(const char* str __rfuncarg__) {
			return _detail::print_const_char_pointer(str);
		}

		func const bool print_string_ln(const char* str __rfuncarg__) {
			return _detail::print_const_char_pointer(str) && print_ln();
		}
	}
}
