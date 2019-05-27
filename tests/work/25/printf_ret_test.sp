import <"std/io">
import <"std/string">
import <"std/aliases">
import "../test.sp"

func int main() {
	using namespace std::aliases;

	auto hello = "hello", world = "world";
	auto ret = io::printf("%s %s\n", hello, world);
	if (ret != str::strlen(hello) + str::strlen(world) + 1 + 1) fail();
	success();
}
