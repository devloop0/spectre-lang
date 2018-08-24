import <"stdx/string">
import <"std/syscall">

func int main() {
	namespace xstr = stdx::string::string;
	namespace ssys = std::syscall;

	const auto s = xstr::new_string("Y\n");
	ssys::direct_write(1, xstr::data(s), xstr::length(s));
	xstr::delete_string(s as type xstr::string*);
	return 0;
}
