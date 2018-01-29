import <"stdx/string">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace std::string;
	using namespace stdx::string;

	type string::string* s1 = string::new_string("asdfkj@QLWKjernqwweRnwe01UL@wer#(1casdkjsdh");
	char* lower = "asdfkj@qlwkjernqwwernwe01ul@wer#(1casdkjsdh",
		upper = "ASDFKJ@QLWKJERNQWWERNWE01UL@WER#(1CASDKJSDH",
		orig = "asdfkj@QLWKjernqwweRnwe01UL@wer#(1casdkjsdh";
	
	type string::string* s1_lower = string::lower(s1), s1_upper = string::upper(s1);
	if(strcmp(s1_lower->c_string, lower) != 0 || strcmp(s1_upper->c_string, upper) != 0 ||
		strcmp(s1->c_string, orig) != 0) fail();

	string::inplace_lower(s1);
	if(strcmp(s1_lower->c_string, lower) != 0 || strcmp(s1_upper->c_string, upper) != 0 ||
		strcmp(s1->c_string, lower) != 0) fail();

	string::inplace_upper(s1);
	if(strcmp(s1_lower->c_string, lower) != 0 || strcmp(s1_upper->c_string, upper) != 0 ||
		strcmp(s1->c_string, upper) != 0) fail();

	success();
}
