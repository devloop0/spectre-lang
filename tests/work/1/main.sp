import <"std/syscall">
import <"std/string">

func int main() {
	using std::string::strlen;
	using std::syscall::exit;
	# exit(1);
	char* name = stk char(256);
	using std::syscall::direct_write;
	using std::syscall::direct_read;
	char* to_write1 = "Please enter your name: ";
	direct_write(1, to_write1, strlen(to_write1));
	direct_read(2, name, 256);
	direct_write(1, name, strlen(name) - 1); # skip the new line
	char* to_write2 = " says \"Hello World!\"\n";
	direct_write(1, to_write2, strlen(to_write2));
	return 0;
}
