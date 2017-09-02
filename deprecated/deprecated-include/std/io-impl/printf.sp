import <"std/io">
import <"std/core">

namespace std {
	namespace io {
		
		func const signed int printf(const char* str,
			const type std::core::primitive* ptl) {
			using namespace std::core;
			unsigned int str_counter = 0, ptl_counter = 0;
			signed int ret = 0;
			while(str[str_counter] != '\0') {
				if(str[str_counter] == '%') {
					str_counter++;
					if(str[str_counter] == '\0') {
						ret = -1;
						break;
					}
					bool hit = true;
					switch(str[str_counter]) {
					case 's': {
						const type primitive* curr = ptl[ptl_counter++]$;
						const char* str = get_string(curr);
						if(curr->error_code == -1)
							hit = false;
						else
							__asm__ ( "li $2, 4" : str "$4" : "lw $4, 0($4)" : "syscall" );
					}
						break;
					case 'u': {
						const type primitive* curr = ptl[ptl_counter++]$;
						unsigned int u = get_uint(curr);
						if(curr->error_code == -1)
							hit = false;
						else
							__asm__ ( "li $2, 36" : u "$4" : "lw $4, 0($4)" : "syscall");
					}
						break;
					case 'c': {
						const type primitive* curr = ptl[ptl_counter++]$;
						const char c = get_char(curr);
						if(curr->error_code == -1)
							hit = false;
						else
							__asm__ ( "li $2, 11" : c "$4" : "lbu $4, 0($4)" : "syscall" );
					}
						break;
					case 'd': {
						const type primitive* curr = ptl[ptl_counter++]$;
						const int i = get_int(curr);
						if(curr->error_code == -1)
							hit = false;
						else
							__asm__ ( "li $2, 1" : i "$4" : "lw $4, 0($4)" : "syscall");
					}
						break;
					case 'f': {
						const type primitive* curr = ptl[ptl_counter++]$;
						double d;
						if(curr->tag == (1 << 11)) d = get_float(curr);
						else if(curr->tag == (1 << 12)) d = get_double(curr);
						else d = 5;
						if(curr->error_code == -1)
							hit = false;
						else
							__asm__ ( "li $2, 3" : d "$8" : "ldc1 $f12, 0($8)" : "syscall" );
					}
						break;
					case '%':
						__asm__ ( "li $2, 11" : "li $4, '%'" : "syscall" );
						break;
					default:
						hit = false;
						break;
					}
					if(!hit) {
						ret = -1;
						break;
					}
				}
				else {
					char temp = str[str_counter];
					__asm__ ( "li $2, 11" : temp "$4" : "lbu $4, 0($4)" : "syscall" );
				}
				str_counter++;
			}
			return ret;
		}
	}
}
