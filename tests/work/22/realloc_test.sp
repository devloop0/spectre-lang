import <"stdx/string">
import <"std/string">

import "../test.sp"

func int main() {
	using namespace stdx::string;
	using namespace std::string;

	constexpr unsigned int LEN = 10'000;
	char* sA = "aa";
	constexpr char cA = 'b';
	char* all = "aab";

	type string::string* s = string::new_string("");

	for(unsigned int i = 0; i < LEN; i++)
		string::concatc(s, sA), string::concatch(s, cA);

	if(string::length(s) != 3 * LEN) fail();

	for(unsigned int i = 0; i < string::length(s); i += 3)
		if(strncmp(string::data(s)[i]$, all, 3) != 0) fail();

	success();
}
