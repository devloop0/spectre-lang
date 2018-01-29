import <"std/string">
import <"std/lib">
import <"std/syscall">

import "../test.sp"

struct string {
	bool perm;
	char* c_string;
	unsigned int size;
}

func type string c_string(char* c) {
	using std::string::strlen;
	
	type string ret;
	ret.perm = false;
	ret.c_string = c;
	ret.size = strlen(c);
	return ret;
}

func type string new_string(const char* c) {
	using std::string::strlen;
	using std::string::strcpy;
	using std::lib::malloc;

	type string ret;
	ret.perm = true;
	ret.c_string = malloc(strlen(c) + 1) as char*;
	ret.size = strlen(c);
	strcpy(ret.c_string, c);
	return ret;
}

func type string concat(type string s1, const type string s2) {
	using std::string::strcat;
	using std::lib::realloc;
	
	if(!s1.perm) return s1;
	s1.c_string = realloc(s1.c_string as byte*, s1.size + s2.size + 1) as char*;
	strcat(s1.c_string, s2.c_string);
	s1.size += s2.size;
	return s1;
}

func type string cconcat(const char* c, type string s1) {
	using std::string::strcpy;
	using std::string::strlen;
	using std::lib::malloc;
	using std::lib::free;
	using std::string::strcat;

	if(!s1.perm) return s1;
	char* temp = malloc(s1.size + strlen(c) + 1) as char*;
	free(s1.c_string as byte*);
	strcpy(temp, c);
	strcat(temp, s1.c_string);
	s1.c_string = temp;
	s1.size += strlen(c);
	return s1;
}

func type string concatc(type string s1, const char* c) {
	using std::string::strlen;
	using std::lib::realloc;
	using std::string::strcat;

	if(!s1.perm) return s1;
	s1.c_string = realloc(s1.c_string as byte*, s1.size + strlen(c) + 1) as char*;
	strcat(s1.c_string, c);
	s1.size += strlen(c);
	return s1;
}

func void free_string(type string s1) {
	if(s1.perm) std::lib::free(s1.c_string as byte*);
}

func void print_string(type string s) {
	std::syscall::direct_write(1, s.c_string, s.size);
}

func void putc(char c) {
	std::syscall::direct_write(1, c$, 1);
}

func int main() {
	type string s1 = c_string("World!"),
		s = new_string("Hello");
	
	print_string(s); putc('\n');
	print_string(s1); putc('\n');

	s = cconcat("Nikhil says, ", concat(
		concatc(s, " the Earth/"), s1));
	if(!s.perm || s1.perm) fail();
	print_string(s); putc('\n');

	free_string(s1);
	free_string(s);
}
