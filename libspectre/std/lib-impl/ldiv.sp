import <"std/lib">

namespace std {
	namespace lib {

		func type ldiv_t ldiv(long num, long denom) {
			type ldiv_t d;
			long quot, rem;
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
