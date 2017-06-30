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
						#break;
					}
					bool hit = true;
					if(str[str_counter] == 's') {
						const type primitive* curr = ptl[ptl_counter++]$;
						const char* str = get_string(curr);
						# __asm__ ( "li $2, 4" : str "$4" : "lw $4, 0($4)" : "syscall" );
						# __asm__ ( "li $2, 1" :  e "$4" : "lw $4, 0($4)" : "syscall" );
						# __asm__ ( "li $2, 11" : "li $4, '1'" : "syscall" );
						bool curr_e = curr->error_code == primitive_constants::ERROR;
						int pe = primitive_constants::ERROR, e = curr->error_code;
						__asm__ ( "li $2, 1" : e "$4" : "lw $4, 0($4)" : "syscall" );
						__asm__ ( "li $2, 1" : pe "$4" : "lw $4, 0($4)" : "syscall" );
						if(curr->error_code == primitive_constants::ERROR)
							hit = false;
						else
							__asm__ ( "li $2, 4" : str "$4" : "lw $4, 0($4)" : "syscall" );
					}
					else if(str[str_counter] == 'c') {
						const type primitive* curr = ptl[ptl_counter++]$;
						const char c = get_char(curr);
						if(curr->error_code == primitive_constants::ERROR)
							hit = false;
						else
							__asm__ ( "li $2, 11" : c "$4" : "lbu $4, 0($4)" : "syscall" );
					}
					if(!hit) {
						ret = -1;
						#break;
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
