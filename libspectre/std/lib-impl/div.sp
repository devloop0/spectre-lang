import <"std/lib">

namespace std {
	namespace lib {

		func type div_t div(int num, int denom) {
			type div_t d;
			int quot, rem;
			__asm__ (
				num "$8" :
				denom "$9" :
				"lw $8, 0($8)" :
				"lw $9, 0($9)" :
				"div $8, $9" :
				quot "$8" :
				rem "$9" :
				"mflo $10" :
				"sw $10, 0($8)" :
				"mfhi $10" :
				"sw $10, 0($9)"
			);
			d.quot = quot, d.rem = rem;
			return d;
		}
	}
}
