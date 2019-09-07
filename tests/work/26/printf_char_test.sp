import <"std/io">

import "../test.sp"

func int main() {
	std::io::printf("%s%c%s%c%s", "Hello", ' ', "World", '!', "\n");
	success();
}
