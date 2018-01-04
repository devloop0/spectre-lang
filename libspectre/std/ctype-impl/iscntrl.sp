import <"std/ctype">

namespace std {
	namespace ctype {

		func bool iscntrl(char c) {
			return (0o000 <= c && c <= 0o037) || c == 0o177;
		}
	}
}
