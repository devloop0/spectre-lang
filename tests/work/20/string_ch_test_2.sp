import <"stdx/string">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace stdx::string;
	using std::string::strcmp;

	type string::string* s = string::new_string("");
	
	const char* h = "Hello World";
	const char* fnal = "^Hello World!";

	char* iter = h as char*;
	while(iter@ as bool)
		string::concatch(s, iter@), iter = iter[1]$;

	type string::string* temp1 = string::addch(s, '!');
	type string::string* temp2 = string::chadd('^', temp1);

	if(strcmp(string::data(temp2), fnal) != 0) fail();

	string::delete_string(temp1);
	string::delete_string(temp2);
	string::delete_string(s);

	success();
}
