import <"std/string">
import <"std/alloca">

import "../test.sp"

namespace str = std::string;
namespace lib = std::lib;

func char* test(char* str1, char* str2) {
	char* name = lib::alloca(str::strlen(str1) + str::strlen(str2) + 1) as char*;
	str::strcpy(str::strcpy(name, str1)[str::strlen(str1)]$, str2);
	return name;
}

func int main() {
	char* t = test("Nikkil ", "Athreya");
	t[3] = 'h';
	if (str::strcmp(t, "Nikhil Athreya") != 0) fail();
	lib::freea(t as byte*);
	success();
}
