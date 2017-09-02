import <"std/syscall">
import <"std/string">

namespace s {
	struct s {
		int my_int;
		fn void(type s*,int) set_my_int;
		fn int(type s*) get_my_int;
	}

	namespace __s_helper {
		func void default_set_my_int(type s* this, int my_int) {
			this->my_int = my_int;
		}

		func int default_get_my_int(type s* this) {
			return this->my_int;
		}
	}

	func type s* init(type s* ss, int my_int) {
		ss->my_int = my_int;
		ss->set_my_int = __s_helper::default_set_my_int;
		ss->get_my_int = __s_helper::default_get_my_int;
		return ss;
	}
}

func type s::s* wrap(type s::s* ss) { return ss; }

func int main() {
	char* c = stk char(4);
	c[1] = '!', c[2] = '\n', c[3] = 0;
	type s::s local;
 	s::init(local$, 8);
	c@ = (((wrap(local$))))->get_my_int(local$) + '0';
	std::syscall::direct_write(1, c, std::string::strlen(c));
	(((((((((wrap)))((((local)))$))))@))).set_my_int(local$, 3);
	c@ = local.get_my_int(local$) + '0';
	std::syscall::direct_write(1, c, std::string::strlen(c));
	return 0;
}

