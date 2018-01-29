import <"stdx/string">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace stdx::string;
	using namespace std::string;
	
	type string::string* s1 = string::new_string("this is a test string exercising find.");
	type string::string* s2 = string::new_string("trin"), s3 = string::new_string("xerci");
	type string::string* u = string::new_string("xerciz");
	const char* cu = "xercisj";
	if(string::cfind(s1, "test") != 10) fail();
	if(string::find(s1, s2) != 16) fail();
	if(string::find_range(s1, s3, 10, 30) != 23) fail();
	if(string::find_range(s1, s3, 26, 34) != -1) fail();
	if(string::find(s1, u) != -1) fail();
	if(string::cfind(s1, cu) != -1) fail();
	if(string::cfind_range(s1, "test", 5, 15) != 10) fail();
	if(string::cfind_range(s1, "test", 11, 17) != -1) fail();

	if(strcmp(s1->c_string, "this is a test string exercising find.") != 0 ||
		strcmp(s2->c_string, "trin") != 0 || strcmp(s3->c_string, "xerci") != 0 ||
		strcmp(u->c_string, "xerciz") != 0 || strcmp(cu, "xercisj") != 0) fail();
	
	success();
}
