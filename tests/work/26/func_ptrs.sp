import <"std/io">
import <"std/lib">

func void v1() { std::io::printf("Added first; should be called second.\n"); }
func void v2() { std::io::printf("Added second; should be called first.\n"); }
func int main() {
	auto t = [ v1, v2 ];
	for (int i = 0; i < 2; i++)
		std::lib::atexit(t[i]);
	std::io::printf("Added everything. Goodbye...\n");
}
