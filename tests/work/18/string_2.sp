import <"stdx/string">
import <"std/syscall">

import "../test.sp"

func void print_string(type stdx::string::string::string* s) {
	using std::syscall::direct_write;

	direct_write(1, s->c_string, s->size);
}

func void putc(char c) {
	using std::syscall::direct_write;

	direct_write(1, c$, 1);
}

func int main(int argc, char** argv) {
	using namespace stdx::string;
	
	type string::string* hello = string::new_string("Hello"),
		space = string::new_string(" "),
		s1 = string::add(hello, space),
		s2 = string::cadd("My name is Nikhil, ", s1),
		s3 = string::addc(s2, "World\n");

	print_string(hello); putc('k'), putc('\n');
	print_string(space); putc('k'), putc('\n');
	print_string(s1); putc('k'), putc('\n');
	print_string(s2); putc('k'), putc('\n');
	print_string(s3);
	
	string::delete_string(s2);
	string::delete_string(s1);
	string::delete_string(s3);
	string::delete_string(space);
	string::delete_string(hello);
}
