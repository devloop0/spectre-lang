import <"std/hooks">
import <"std/lib">

namespace std {
	namespace hooks {
		func unsigned int uidiv(unsigned int n, unsigned int d) {
			if (d == 0)
				std::lib::abort();

			unsigned int b = 1;
			while ((n >> 1) >= d)
				b <<= 1, d <<= 1;

			unsigned int r = 0;
			while (b > 0) {
				if (n >= d)
					n -= d, r += b;
				b >>= 1, d >>= 1;
			}
			return r;
		}

		func int idiv(int n, int d) {
			bool negate = false;
			if (d == 0 || (n == 0x80'00'00'00 && d == -1))
				std::lib::abort();

			if (n < 0) {
				negate = !negate;
				n = -n;
			}

			if (d < 0) {
				negate = !negate;
				d = -d;
			}

			int q = uidiv(n, d);
			if (negate) q = -q;
			return q;
		}

		func unsigned int uimod(unsigned int n, unsigned int d) {
			unsigned int q = uidiv(n, d);
			return n - (q * d);
		}

		func int imod(int n, int d) {
			int q = idiv(n, d);
			return n - (q * d);
		}
	}
}
