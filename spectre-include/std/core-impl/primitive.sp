import <"std/core">

namespace std {	
	namespace core {
	
		func type primitive primitive_signed_int(const signed int s __rfuncarg__) {
			type primitive pt;
			int* p = pt.__dummy$;
			__asm__ (
				p "$8" :
				"lw $8, 0($8)" :
				"sw $4, 0($8)"
			);
			pt.tag = primitive_constants::SIGNED_INT;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func const signed int get_signed_int(const type primitive pt) {
			if(pt.tag != primitive_constants::SIGNED_INT) {
				pt.error_code = primitive_constants::ERROR;
				return 0;
			}
			int* d = pt.__dummy$;
			int ret;
			__asm__ (
				d "$8" :
				ret "$9" :
				"lw $8, 0($8)" :
				"lw $8, 0($8)" :
				"sw $8, 0($9)"
			);
			return ret;
		}

		func type primitive primitive_int(const signed int s __rfuncarg__) {
			return primitive_signed_int(s);
		}

		func const signed int get_int(const type primitive pt) {
			return get_signed_int(pt);
		}

		func type primitive primitive_unsigned_int(const unsigned int u __rfuncarg__) {
			type primitive pt;
			int* p = pt.__dummy$;
			__asm__ (
				p "$8" :
				"lw $8, 0($8)" :
				"sw $4, 0($8)"
			);
			pt.tag = primitive_constants::UNSIGNED_INT;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func const unsigned int get_unsigned_int(const type primitive pt) {
			if(pt.tag != primitive_constants::UNSIGNED_INT) {
				pt.error_code = primitive_constants::ERROR;
				return 0;
			}
			int* d = pt.__dummy$;
			unsigned int ret;
			__asm__ (
				d "$8" :
				ret "$9" :
				"lw $8, 0($8)" :
				"lw $8, 0($8)" :
				"sw $8, 0($9)"
			);
			return ret;
		}

		func type primitive primitive_uint(const unsigned int u __rfuncarg__) {
			return primitive_unsigned_int(u);
		}

		func const unsigned int get_uint(const type primitive pt) {
			return get_unsigned_int(pt);
		}
	}
}
