import <"std/syscall">

namespace test {
	namespace t = std::syscall;
}

using namespace test::t;

func int main() {
	direct_write(1, "Y\n", 2);
	return 0;
}
