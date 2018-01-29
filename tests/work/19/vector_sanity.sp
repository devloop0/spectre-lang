import <"stdx/vector">
import <"std/string">

import "../test.sp"

using namespace stdx::vector;
using std::string::strcmp;

func int main() {
	type vector::vector* v = vector::new_vector_capacity(sizeof{char*}, 1);
	char** argv = [ "asdf", "sd", "sdq", "qwerqwe", "asd", "qwe", "qwercq", "sadx", "qwec", "qwe", "qwe", "qwe",
			"q1wdf", "qsd", "ed", "1eqw", "1ef1" ];
	int argc = 17;
	
	for(int i = argc - 1; i > 0; i--) vector::append(v, argv[i]$ as byte*);
	for(int i = 1; i < argc; i++) if(strcmp(vector::at(v, (argc - 1) - i) as char** @, argv[i]) != 0) fail();
	
	if(strcmp(vector::at(v, 4) as char** @, "q1wdf") != 0) fail();
	
	vector::delete_vector(v);
	success();
}
