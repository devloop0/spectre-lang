import <"std/lib">
import <"std/ctype">

namespace std {
	namespace lib {

		func long atol(const char* str) {
			using std::ctype::isspace;
			using std::ctype::isdigit;

			char* s = str as char*;
			long ret = 0, sign = 1;
			
			while(isspace(s@)) s = s[1]$;

			if(s@ == '-' || s@ == '+') {
				sign = s@ == '-' ? -1 : 1;
				s = s[1]$;
			}

			while(s@ as bool) {
				if(!isdigit(s@)) return ret * sign;
				ret *= 10, ret += s@ - '0';
				s = s[1]$;
			}
			return sign * ret;
		}
	}
}
