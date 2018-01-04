import <"std/syscall">
import <"std/lib">

func int main() {
	byte* temp = std::syscall::curr_brk();
	byte* b = std::syscall::raw_brk(temp[1]$);
	char* c = b as char*;
	std::syscall::direct_write(1, (b@ = 0 + '0', c), 1);
	std::syscall::direct_write(1, (b@ = 9 + '0', c), 1);
	std::syscall::direct_write(1, (c@ = '\n', c), 1);
}
