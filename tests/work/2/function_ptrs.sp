import <"std/syscall">
import <"std/string">

fn char*() * GLOBAL = resv fn char*()(4);

func char* f() { return "Nikhil\n"; }
func char* g() { return "Athreya\n"; }
func char* h() { return "is\n"; }
func char* i() { return "great\n"; }

func int main() {
	GLOBAL@ = f;
	GLOBAL[1] = g;
	GLOBAL[2] = h;
	GLOBAL[3] = i;
	
	for(int i = 0; i < 4; i++) {
		char* res = GLOBAL[i]();
		std::syscall::direct_write(1, res, std::string::strlen(res));
	}
	return 0;
}
