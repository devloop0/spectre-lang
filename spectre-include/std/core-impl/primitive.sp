import <"std/core">

namespace std {	
	namespace core {
	
		namespace _detail {

			func[static] type primitive primitive_signed_int(const signed int s) {
				type primitive pt;
				int* p = pt.__dummy$;
				__asm__ (
					p "$8" :
					"lw $8, 0($8)" :
					"sw $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::SIGNED_INT;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const signed int get_signed_int(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::SIGNED_INT) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				int ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lw $8, 0($8)" :
					"sw $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_int(const signed int s) {
			return _detail::primitive_signed_int(s);
		}

		func const signed int get_int(const type primitive* pt) {
			return _detail::get_signed_int(pt);
		}

		namespace _detail {

			func[static] type primitive primitive_unsigned_int(const unsigned int u) {
				type primitive pt;
				int* p = pt.__dummy$;
				__asm__ (
					p "$8" :
					"lw $8, 0($8)" :
					"sw $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::UNSIGNED_INT;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const unsigned int get_unsigned_int(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::UNSIGNED_INT) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				unsigned int ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lw $8, 0($8)" :
					"sw $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_uint(const unsigned int u) {
			return _detail::primitive_unsigned_int(u);
		}

		func const unsigned int get_uint(const type primitive* pt) {
			return _detail::get_unsigned_int(pt);
		}

		namespace _detail {
	
			func[static] type primitive primitive_signed_byte(const signed byte b) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sb $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::SIGNED_BYTE;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}
	
			func[static] const signed byte get_signed_byte(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::SIGNED_BYTE) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				signed byte ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lb $8, 0($8) " :
					"sb $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_byte(const signed byte b) {
			return _detail::primitive_signed_byte(b);
		}

		func const signed byte get_byte(const type primitive* pt) {
			return _detail::get_signed_byte(pt);
		}

		namespace _detail {

			func[static] type primitive primitive_unsigned_byte(const unsigned byte u) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sb $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::UNSIGNED_BYTE;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const unsigned byte get_unsigned_byte(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::UNSIGNED_BYTE) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				unsigned byte ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lbu $8, 0($8)" :
					"sb $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_ubyte(const unsigned byte u) {
			return _detail::primitive_unsigned_byte(u);
		}

		func const unsigned byte get_ubyte(const type primitive* pt) {
			return _detail::get_unsigned_byte(pt);
		}

		namespace _detail {
			
			func[static] type primitive primitive_signed_char(const signed char c) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sb $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::SIGNED_CHAR;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const signed char get_signed_char(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::SIGNED_CHAR) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				signed char ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lb $8, 0($8)" :
					"sb $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_char(const signed char c) {
			return _detail::primitive_signed_char(c);
		}

		func const signed char get_char(const type primitive* pt) {
			return _detail::get_signed_char(pt);
		}

		namespace _detail {

			func[static] type primitive primitive_unsigned_char(const unsigned char u) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sb $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::UNSIGNED_CHAR;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const unsigned char get_unsigned_char(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::UNSIGNED_CHAR) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				unsigned char ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lbu $8, 0($8)" :
					"sb $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_uchar(const unsigned char u) {
			return _detail::primitive_unsigned_char(u);
		}

		func const unsigned char get_uchar(const type primitive* pt) {
			return _detail::get_unsigned_char(pt);
		}

		func type primitive primitive_bool(const bool b) {
			type primitive pt;
			int* d = pt.__dummy$;
			__asm__ (
				d "$8" :
				"lw $8, 0($8)" :
				"sb $4, 0($8)"
			);
			pt.tag = primitive_constants::BOOL;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func const bool get_bool(const type primitive* pt) {
			if(pt->tag != primitive_constants::BOOL) {
				pt->error_code = primitive_constants::ERROR;
				return false;
			}
			int* d = pt->__dummy$;
			bool ret;
			__asm__ (
				d "$8" :
				ret "$9" :
				"lw $8, 0($8)" :
				"lbu $8, 0($8)" :
				"sb $8, 0($9)"
			);
			return ret;
		}

		namespace _detail {

			func[static] type primitive primitive_signed_short(const signed short s) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sh $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::SIGNED_SHORT;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const signed short get_signed_short(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::SIGNED_SHORT) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				signed short ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lh $8, 0($8)" :
					"sh $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_short(const signed short s) {
			return _detail::primitive_signed_short(s);
		}

		func const signed short get_short(const type primitive* pt) {
			return _detail::get_signed_short(pt);
		}

		namespace _detail {

			func[static] type primitive primitive_unsigned_short(const unsigned short u) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sh $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::UNSIGNED_SHORT;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const unsigned short get_unsigned_short(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::UNSIGNED_SHORT) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				unsigned short ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lhu $8, 0($8)" :
					"sh $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_ushort(const unsigned short u) {
			return _detail::primitive_unsigned_short(u);
		}

		func const unsigned short get_ushort(const type primitive* pt) {
			return _detail::get_unsigned_short(pt);
		}

		namespace _detail {
		
			func[static] type primitive primitive_signed_long(const signed long s) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sw $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::SIGNED_LONG;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}
			
			func[static] const signed long get_signed_long(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::SIGNED_LONG) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				signed long ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lw $8, 0($8)" :
					"sw $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_long(const signed long s) {
			return _detail::primitive_signed_long(s);
		}

		func const signed long get_long(const type primitive* pt) {
			return _detail::get_signed_long(pt);
		}

		namespace _detail {

			func[static] type primitive primitive_unsigned_long(const unsigned long u) {
				type primitive pt;
				int* d = pt.__dummy$;
				__asm__ (
					d "$8" :
					"lw $8, 0($8)" :
					"sw $4, 0($8)"
				);
				pt.tag = ::std::core::primitive_constants::UNSIGNED_LONG;
				pt.error_code = ::std::core::primitive_constants::NO_ERROR;
				return pt;
			}

			func[static] const unsigned long get_unsigned_long(const type primitive* pt) {
				if(pt->tag != ::std::core::primitive_constants::UNSIGNED_LONG) {
					pt->error_code = ::std::core::primitive_constants::ERROR;
					return 0;
				}
				int* d = pt->__dummy$;
				unsigned long ret;
				__asm__ (
					d "$8" :
					ret "$9" :
					"lw $8, 0($8)" :
					"lw $8, 0($8)" :
					"sw $8, 0($9)"
				);
				pt->error_code = ::std::core::primitive_constants::NO_ERROR;
				return ret;
			}
		}

		func type primitive primitive_ulong(const unsigned long u) {
			return _detail::primitive_unsigned_long(u);
		}

		func const unsigned long get_ulong(const type primitive* pt) {
			return _detail::get_unsigned_long(pt);
		}

		func const unsigned int bits_from_float(const float f) {
			unsigned int ret;
			__asm__ (
				ret "$9" :
				"swc1 $f12, 0($9)"
			);
			return ret;
		}
	
		func const float float_from_bits(const unsigned int u) {
			float ret;
			__asm__ (
				ret "$9" :
				"mtc1 $4, $f4" :
				"swc1 $f4, 0($9)"
			);
			return ret;
		}

		func type primitive primitive_float(const float f) {
			type primitive pt;
			pt.__dummy = bits_from_float(f);
			pt.tag = 1 << 11;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func const float get_float(const type primitive* pt) {
			if(pt->tag != (1 << 11)) {
				pt->error_code = std::core::primitive_constants::ERROR;
				return 0;
			}
			pt->error_code = std::core::primitive_constants::NO_ERROR;
			return float_from_bits(pt->__dummy);
		}

		func type primitive primitive_double(const double d) {
			type primitive pt;
			int* dp = pt.__dummy$;
			__asm__ (
				dp "$8" :
				"lw $8, 0($8)" :
				"sdc1 $f12, 0($8)"
			);
			pt.tag = 1 << 12;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func const double get_double(const type primitive* pt) {
			if(pt->tag != (1 << 12)) {
				pt->error_code = primitive_constants::ERROR;
				return 0;
			}
			int* d = pt->__dummy$;
			double ret;
			__asm__ (
				d "$8" :
				ret "$9" :
				"lw $8, 0($8)" :
				"addiu $10, $8, 4" :
				"lw $11, 0($8)" :
				"mtc1 $11, $f4" :
				"lw $11, 0($10)" :
				"mtc1 $11, $f5" :
				"sdc1 $f4, 0($9)"
			);
			pt->error_code = primitive_constants::NO_ERROR;
			return ret;
		}

		func type primitive primitive_null() {
			type primitive pt;
			pt.__dummy = 0;
			pt.tag = primitive_constants::NULL;
			pt.error_code = primitive_constants::NO_ERROR;
			return pt;
		}

		func type primitive primitive_string(const char* str) {
			type primitive ret;
			ret.__dummy = str as unsigned int;
			ret.tag = primitive_constants::STRING;
			return ret;
		}

		func const char* get_string(const type primitive* pt) {
			if(pt->tag != primitive_constants::STRING) {
				pt->error_code = primitive_constants::ERROR;
				return char_nullptr;
			}
			char* str = pt->__dummy as char*;
			pt->error_code = primitive_constants::NO_ERROR;
			return str;
		}
	}
}
