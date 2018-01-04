import <"std/lib">
import <"std/math">
import <"std/ctype">
import <"std/string">

namespace std {
	namespace lib {

		func double atof(const char* str) {
			using std::ctype::isspace;
			using std::ctype::tolower;
			using std::ctype::isalpha;
			using std::ctype::isxdigit;
			using std::ctype::isdigit;
			using std::string::strlen;
			using std::math::infinity;
			using std::math::nan;

			char* s = str as char*;
			char* inf1 = "inf", inf2 = "infinity", nan1 = "nan";

			while(isspace(s@)) s = s[1]$;

			double sign = 1.;
			if(s@ == '+' || s@ == '-') {
				sign = s@ == '-' ? -1 : 1;
				s = s[1]$;
			}

				
			if(tolower(s@) == 'i') {
				if(strlen(s) == 3) {
					while(s@ as bool) {
						if(tolower(s@) != tolower(inf1@)) return 0.;
						s = s[1]$, inf1 = inf1[1]$;
					}
					return sign * infinity();
				}
				else if(strlen(s) == 8) {
					while(s@ as bool) {
						if(tolower(s@) != tolower(inf2@)) return 0.;
						s = s[1]$, inf2 = inf2[1]$;
					}
					return sign * infinity();
				}
				return 0.;
			}
			else if(tolower(s@) == 'n') {
				for(int i = 0; i < 3; i++) {
					if(tolower(s@) != tolower(nan1@)) return 0.;
					s = s[1]$, nan1 = nan1[1]$;
				}
				double n = sign * nan(), c = 0.;
				if(!s@ as bool) return n;
				if(s@ != '(') return n;
				s = s[1]$;
				while(s@ as bool && s@ != ')') {
					if(!isxdigit(s@)) return n;
					c *= 16;
					if(isdigit(s@)) c += s@ - '0';
					else c += (tolower(s@) - 'a') + 10;
					s = s[1]$;
				}
				if(s@ != ')') return n;
				s = s[1]$;
				if(!s@ as bool) return n + c;
				else return n;
			}

			double number = 0., decimal = 0., exponent = 0.;
			bool digit_hit = false, e_hit = false, p_hit = false, decimal_hit = false;
			double e_sign = 1., p_sign = 1.;
			bool hex = s@ as bool && s@ == '0' ? tolower(s[1]) == 'x' : false;
			if(hex) s = s[2]$;
			double decimal_divider = hex ? 16. : 10.;
			while(s@ as bool) {
				if(p_hit || e_hit) {
					if(!isdigit(s@)) break;
					exponent *= 10, exponent += s@ - '0';
				}
				else if(hex) {
					if(!isxdigit(s@) && s@ != 'p' && s@ != '.')
						break;
					if(s@ == 'p') {
						if(!digit_hit) break;
						p_hit = true;
						if(s[1] as bool) {
							if(s[1] == '+' || s[1] == '-') {
								p_sign = s[1] == '-' ? -1 : 1;
								s = s[1]$;
							}
						}
					}
					else {
						if(isdigit(s@)) {
							digit_hit = true;
							if(!decimal_hit)
								number *= 16, number += s@ - '0';
							else
								decimal += (s@ - '0') / decimal_divider,
									decimal_divider *= 16;
						}
						else if(s@ != '.') {
							digit_hit = true;
							if(!decimal_hit)
								number *= 16, number += (tolower(s@) - 'a') + 10;
							else
								decimal += ((tolower(s@) - 'a') + 10) / decimal_divider,
									decimal_divider *= 16;
						}
						else {
							if(decimal_hit) break;
							decimal_hit = true;
						}
					}
				}
				else {
					if(!isdigit(s@) && s@ != 'e' && s@ != '.')
						break;
					if(s@ == 'e') {
						if(!digit_hit) break;
						e_hit = true;
						if(s[1] as bool) {
							if(s[1] == '+' || s[1] == '-') {
								e_sign = s[1] == '-' ? -1 : 1;
								s = s[1]$;
							}
						}
					}
					else {
						if(isdigit(s@)) {
							digit_hit = true;
							if(!decimal_hit)
								number *= 10, number += s@ - '0';
							else
								decimal += (s@ - '0') / decimal_divider,
									decimal_divider *= 10;
						}
						else {
							if(decimal_hit) break;
							decimal_hit = true;
						}
					}
				}
				s = s[1]$;
			}

			double true_exponent = 1.;
			while(exponent-- != 0) {
				if(e_hit) true_exponent *= 10.;
				else if(p_hit) true_exponent *= 2.;
			}

			if(e_sign != -1 && p_sign != -1)
				return sign * (number + decimal) * true_exponent;
			else 
				return sign * (number + decimal) / true_exponent;
		}
	}
}
