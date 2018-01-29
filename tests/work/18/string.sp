import <"stdx/string">
import <"std/syscall">

func void print_string(type stdx::string::string::string* s) {
	std::syscall::direct_write(1, s->c_string, s->size);
}

func int main() {
	using namespace stdx::string;

	type string::string* s = string::new_string("Hello"),
		nl = string::new_string("\n");
	s = string::cconcat("My name is Nikhil, ", string::concatc(s, " World!"));
	s = string::concat(s, nl);
	print_string(s);
	string::delete_string(nl), string::delete_string(s);
}
