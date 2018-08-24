import <"stdx/string">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace stdx::string;
	using std::string::strlen;
	using std::string::strcmp;
	
	const char* h = "Hello World!\n";

	type string::string* s1 = string::new_string(""), s2 = string::new_string("");
	for(unsigned int i = 0; i < strlen(h); i++)
		string::concatch(s1, h[i]);

	for(int i = strlen(h) - 1; i >= 0; i--)
		string::chconcat(h[i], s2);

	if(string::length(s1) != string::length(s2)) fail();

	if(strcmp(string::data(s1), string::data(s2)) != 0) fail();

	for(unsigned int i = 0; i < string::length(s1); i++)
		if(string::at(s1, i) != string::at(s2, i)) fail();

	success();
}
