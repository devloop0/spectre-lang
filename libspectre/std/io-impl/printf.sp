import <"std/io">
import <"std/limits">
import <"std/syscall">
import <"std/string">
import <"std/arg">
import <"std/aliases">

namespace std {
	namespace io {

		func int printf(const char* _fmt, ...) {
			using namespace std::aliases;
		
			unsigned int ret = 0;
			type arg::va_list* args = arg::va_start(arg::align(sizeof(_fmt)), _fmt$ as byte*);
			const char* int_min = "-2147483648";
			unsigned int int_min_len = str::strlen(int_min);
			char minus = '-', zero = '0';
			char* fmt = _fmt as char*;
			while(fmt@ as bool) {
				if (fmt@ == '%') {
					if (!fmt[1] as bool) break;
					else {
						switch(fmt[1]) {
						case '%':
							sys::direct_write(1, fmt[1]$, 1), ret++;
							break;
						case 'i':
						case 'd': {
							int i;
							arg::va_arg(args, i$ as byte*, sizeof{int});
							if (i == lims::INT_MIN)
								sys::direct_write(1, int_min, int_min_len), ret += int_min_len;
							else {
								if (i < 0) i *= -1, sys::direct_write(1, minus$, 1);
								char* buf = stk char(10), iter = buf;
								while (i as bool) {
									iter@ = i % 10 + '0';
									i /= 10;
									iter = iter[1]$;
								}
								iter = iter[-1]$;
								if (iter == buf[-1]$) sys::direct_write(1, zero$, 1), ret++;
								else while (iter != buf[-1]$) sys::direct_write(1, iter, 1), ret++, iter = iter[-1]$;
							}
						}
							break;
						case 's': {
							char* string;
							arg::va_arg(args, string$ as byte*, sizeof{char*});
							unsigned int l = str::strlen(string);
							sys::direct_write(1, string, str::strlen(string));
							ret += l;
						}
							break;
						case 'u': {
							unsigned int u;
							arg::va_arg(args, u$ as byte*, sizeof{unsigned int});
							char* buf = stk char(12), iter = buf;
							while (u as bool) {
								iter@ = u % 10 + '0';
								u /= 10;
								iter = iter[1]$;
							}
							iter = iter[-1]$;
							if (iter == buf[-1]$) sys::direct_write(1, zero$, 1), ret++;
							else while (iter != buf[-1]$) sys::direct_write(1, iter, 1), ret++, iter = iter[-1]$;
						}
							break;
						case 'p': // pointers are 32-bit, so they are the same size as an unsigned int on this platform
						case 'x':
						case 'X': {
							bool upper = fmt[1] == 'X';
							unsigned int u;
							arg::va_arg(args, u$ as byte*, sizeof{unsigned int});
							char* buf = stk char(12), iter = buf;
							while (u as bool) {
								unsigned char digit = u & 0xf;
								iter@ = digit < 10 ? (digit + '0') : ((digit - 10) + (upper ? 'A' : 'a'));
								u >>= 4;
								iter = iter[1]$;
							}
							iter = iter[-1]$;
							if (iter == buf[-1]$) sys::direct_write(1, zero$, 1), ret++;
							else while (iter != buf[-1]$) sys::direct_write(1, iter, 1), ret++, iter = iter[-1]$;
						}
							break;
						case 'o': {
							unsigned int u;
							arg::va_arg(args, u$ as byte*, sizeof{unsigned int});
							char* buf = stk char(16), iter = buf;
							while (u as bool) {
								unsigned int digit = u & 7;
								iter@ = digit + '0';
								u >>= 3;
								iter = iter[1]$;
							}
							iter = iter[-1]$;
							if (iter == buf[-1]$) sys::direct_write(1, zero$, 1), ret++;
							else while (iter != buf[-1]$) sys::direct_write(1, iter, 1), ret++, iter = iter[-1]$;
						}
							break;
						default:
							break;
						}
						fmt = fmt[1]$;
					}
				}
				else sys::direct_write(1, fmt, 1), ret++;
				fmt = fmt[1]$;
			}
			arg::va_end(args);
			return ret;
		}
	}
}

