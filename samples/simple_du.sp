import <"std/syscall">
import <"stdx/vector">
import <"std/string">
import <"std/lib">
import <"std/io">

import "ltoa.sp"

const unsigned int TABS_TO_SPACES = 8;
const char* B = "B";

func unsigned int div_round_up(const unsigned int i,
	const unsigned int j) {
	return (i + (j - 1)) / j;
}

func int main(int argc, char** argv) {
	using namespace stdx::vector;
	using std::syscall::stat;
	using std::syscall::direct_stat;
	using std::syscall::direct_write;
	using std::string::strlen;
	using std::lib::free;

	if(argc == 1) return 0;

	int ret_code = 0;
	char nl = '\n', t = '\t', sp = ' ';
	type vector::vector* files = vector::new_vector(sizeof{char*}),
		sizes = vector::new_vector(sizeof{char*});

	unsigned int max_tabs = 0;
	for(unsigned int i = 1; i < argc; i++) {
		vector::append(files, argv[i]$ as byte*);
		type stat statbuf;
		const int err = direct_stat(argv[i], statbuf$);
		if(err as bool) ret_code = 1;
		const unsigned long sz = err == 0 ? statbuf.st_size : 0;
		char* str_sz = ltoa(sz);
		vector::append(sizes, str_sz$ as byte*);
		const unsigned int path_len = strlen(argv[i]);
		const unsigned int tabs = div_round_up(path_len, TABS_TO_SPACES);
		if(max_tabs < tabs + 1) max_tabs = tabs + 1;
	}

	if(vector::size(sizes) != vector::size(files)) return 0;

	for(unsigned int i = 0; i < vector::size(sizes); i++) {
		char* file = vector::at(files, i) as char** @,
			sz = vector::at(sizes, i) as char** @;
		unsigned int path_len = strlen(file),
			tabs = div_round_up(path_len, TABS_TO_SPACES);
		direct_write(1, file, path_len);
		for(unsigned int i = (path_len % TABS_TO_SPACES == 0 ? tabs + 1 : tabs);
			i < max_tabs; i++)
			direct_write(1, t$, 1);
		direct_write(1, sz, strlen(sz));
		free(sz as byte*);
		direct_write(1, sp$, 1);
		direct_write(1, B, strlen(B));
		direct_write(1, nl$, 1);
	}

	vector::delete_vector(sizes);
	vector::delete_vector(files);

	return ret_code;
}
