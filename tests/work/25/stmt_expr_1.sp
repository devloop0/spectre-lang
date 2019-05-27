import <"std/io">
import "../test.sp"

func int main() {
	namespace io = std::io;

	int x = ({ io::printf("test\n"); int y = 2; y * y;})
		+ ({ io::printf("test\n"); 5; });
	if (x != 9) fail();
	success();
}
