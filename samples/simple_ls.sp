import <"std/syscall">
import <"stdx/vector">
import <"stdx/string">
import <"std/lib">
import <"std/string">
import <"std/ctype">

import "ltoa.sp"

using namespace stdx::vector;
using namespace stdx::string;

const char* simple_ls_usage = "Usage: ./simple_ls <directory or file name>\n";
const char* missing_or_perm = "Non-existent file/directory or insufficient permissions.\n";
const char* file_or_directory = "Use simple_ls for regular files and directories.\n";
const char path_sep = '/';
const char* empty_krwx = "----------";
const char* question = "??";
const char* curr_dir = ".", parent_dir = "..";
const char* total_sp = "total ";
const char* bold_green_code = "\033[1m\033[32m";
const char* bold_blue_code = "\033[1m\033[34m";
const char* regular_white_code = "\033[0m";

struct print_info {
	char* perm_string,
	      nlink_string,
	      size_string,
	      name;
	unsigned int st_mode;
}

func void regular_white();
func void display_print_info(type print_info* pi, unsigned int max_nlink_len, unsigned int max_size_len);
func int print_info_cmp(const byte* a, const byte* b);
func void update_type(char d_type, char* krwx);
func void update_type_statbuf(int mode, char* krwx);
func void update_perms(char* krwx, type std::syscall::stat statbuf);
func int ls_dir(char* dir);
func int ls_file(char* file, type std::syscall::stat statbuf);

func void display_print_info(type print_info* pi, unsigned int max_nlink_len, unsigned int max_size_len) {
	using namespace std::syscall;
	using std::lib::free;
	using std::string::strlen;

	char sp = ' ', nl = '\n';
	direct_write(1, pi->perm_string, strlen(pi->perm_string));
	free(pi->perm_string as byte*);
	direct_write(1, sp$, 1);

	for(unsigned int i = strlen(pi->nlink_string); i < max_nlink_len; i++)
		direct_write(1, sp$, 1);
	direct_write(1, pi->nlink_string, strlen(pi->nlink_string));
	free(pi->nlink_string as byte*);
	direct_write(1, sp$, 1);

	for(unsigned int i = strlen(pi->size_string); i < max_size_len; i++)
		direct_write(1, sp$, 1);
	direct_write(1, pi->size_string, strlen(pi->size_string));
	free(pi->size_string as byte*);
	direct_write(1, sp$, 1);

	if (S_ISDIR(pi->st_mode))
		direct_write(1, bold_blue_code, strlen(bold_blue_code));
	else if ((pi->st_mode & S_IXUSR) != 0)
		direct_write(1, bold_green_code, strlen(bold_green_code));
	direct_write(1, pi->name, strlen(pi->name));
	direct_write(1, regular_white_code, strlen(regular_white_code));
	free(pi->name as byte*);

	free(pi as byte*);
	direct_write(1, nl$, 1);
}

func int print_info_cmp(const byte* a, const byte* b) {
	using std::string::strcmp;
	using std::ctype::tolower;

	type print_info* p1 = a as type print_info** @;
	type print_info* p2 = b as type print_info** @;
	
	char* s1 = p1->name@ == '.' ? p1->name[1]$ : p1->name;
	char* s2 = p2->name@ == '.' ? p2->name[1]$ : p2->name;
	while (tolower(s1@) == tolower(s2@)) {
		if (s1@ == 0) {
			return 0;
			break;
		}
		s1 = s1[1]$, s2 = s2[1]$;
	}
	return tolower(s1@) - tolower(s2@);
}

func void update_type(char d_type, char* krwx) {
	using namespace std::syscall;

	switch(d_type) {
		case DT_REG: break;
		case DT_FIFO: krwx@ = 'f'; break;
		case DT_CHR: krwx@ = 'c'; break;
		case DT_DIR: krwx@ = 'd'; break;
		case DT_BLK: krwx@ = 'b'; break;
		case DT_LNK: krwx@ = 'l'; break;
		case DT_SOCK: krwx@ = 's'; break;
		default: krwx@ = '?'; break;
	}
}

func void update_type_statbuf(int mode, char* krwx) {
	using namespace std::syscall;
	if(S_ISREG(mode)) {}
	else if(S_ISFIFO(mode)) krwx@ = 'f';
	else if(S_ISCHR(mode)) krwx@ = 'c';
	else if(S_ISDIR(mode)) krwx@ = 'd';
	else if(S_ISBLK(mode)) krwx@ = 'b';
	else if(S_ISLNK(mode)) krwx@ = 'l';
	else if(S_ISSOCK(mode)) krwx@ = 's';
	else krwx@ = '?';
}

func void update_perms(char* krwx, type std::syscall::stat statbuf) {
	using namespace std::syscall;

	if((statbuf.st_mode & S_IRUSR) as bool) krwx[1] = 'r';
	if((statbuf.st_mode & S_IWUSR) as bool) krwx[2] = 'w';
	if((statbuf.st_mode & S_IXUSR) as bool) krwx[3] = 'x';
	if((statbuf.st_mode & S_IRGRP) as bool) krwx[4] = 'r';
	if((statbuf.st_mode & S_IWGRP) as bool) krwx[5] = 'w';
	if((statbuf.st_mode & S_IXGRP) as bool) krwx[6] = 'x';
	if((statbuf.st_mode & S_IROTH) as bool) krwx[7] = 'r';
	if((statbuf.st_mode & S_IWOTH) as bool) krwx[8] = 'w';
	if((statbuf.st_mode & S_IXOTH) as bool) krwx[9] = 'x';
}

func int ls_dir(char* dir) {
	using namespace std::syscall;
	using namespace stdx::string;
	using namespace stdx::vector;
	using std::string::strcpy;
	using std::string::strlen;
	using std::lib::malloc;
	using std::lib::calloc;
	using std::lib::free;
	using std::string::strcmp;
	using std::lib::qsort;

	int fd = direct_openat(AT_FDCWD, dir, O_RDONLY | O_DIRECTORY);
	if(fd == -1) {
		direct_write(1, missing_or_perm, strlen(missing_or_perm));
		return 1;
	}

	char nl = '\n';
	unsigned long total_blocks = 0;
	unsigned int max_nlink_len = 0, max_size_len = 0;
	type vector::vector* mems = vector::new_vector(sizeof{type print_info*});
	type vector::vector* dots = vector::new_vector(sizeof{type print_info*});
	while(true) {
		byte* buf = stk byte(1024);
		int nread = direct_getdents(fd, buf, 1024);
		if(nread == -1) {
			direct_write(1, missing_or_perm, strlen(missing_or_perm));
			return 1;
		}
		if(nread == 0) break;

		type print_info* pi = malloc(sizeof{type print_info}) as type print_info*;
		char* d_name = buf[LINUX_DIRENT_d_name_OFFSET]$ as char*;
		unsigned short d_reclen = buf[LINUX_DIRENT_d_reclen_OFFSET]$ as unsigned short* @;
		long d_off = buf[LINUX_DIRENT_d_off_OFFSET]$ as long* @;
		char d_type = buf[d_reclen - 1]$ as char* @;
		direct_lseek(fd, d_off, SEEK_SET);

		char* d_name_copy = malloc(sizeof{char} * (strlen(d_name) + 1)) as char*;
		strcpy(d_name_copy, d_name);
		pi->name = d_name_copy;

		type string::string* qualified_name = string::new_string(dir);
		string::concatch(qualified_name, path_sep);
		string::concatc(qualified_name, d_name);
		type stat statbuf;
		int ret = direct_stat(string::data(qualified_name), statbuf$);
		string::delete_string(qualified_name);
		char* krwx = calloc((strlen(empty_krwx) + 1), sizeof{char}) as char*;
		strcpy(krwx, empty_krwx);
		update_type(d_type, krwx);
		if(ret == -1) {
			pi->perm_string = krwx;
			char* q1 = malloc(sizeof{char} * (strlen(question) + 1)) as char*,
				q2 = malloc(sizeof{char} * (strlen(question) + 1)) as char*;
			strcpy(q1, question);
			strcpy(q2, question);
			pi->nlink_string = q1;
			pi->size_string = q2;
			pi->st_mode = 0;
		}
		else {
			total_blocks += statbuf.st_blocks;
			update_perms(krwx, statbuf);
			pi->perm_string = krwx;
			pi->nlink_string = ltoa(statbuf.st_nlink);
			pi->size_string = ltoa(statbuf.st_size);
			pi->st_mode = statbuf.st_mode;
		}
		if(strlen(pi->nlink_string) > max_nlink_len)
			max_nlink_len = strlen(pi->nlink_string);
		if(strlen(pi->size_string) > max_size_len)
			max_size_len = strlen(pi->size_string);

		if(strcmp(d_name, curr_dir) == 0)
			vector::uint_insert(dots, 0, pi$ as byte*);
		else if(strcmp(d_name, parent_dir) == 0)
			vector::append(dots, pi$ as byte*);
		else
			vector::append(mems, pi$ as byte*);
	}

	direct_write(1, total_sp, strlen(total_sp));
	char* total_blocks_str = ltoa(total_blocks);
	direct_write(1, total_blocks_str, strlen(total_blocks_str));
	direct_write(1, nl$, 1);
	free(total_blocks_str as byte*);

	qsort(vector::data(mems), vector::size(mems), vector::unit(mems), print_info_cmp);

	for(unsigned int i = 0; i < vector::size(dots); i++)
		display_print_info(vector::at(dots, i) as type print_info** @, max_nlink_len, max_size_len);
	for(unsigned int i = 0; i < vector::size(mems); i++)
		display_print_info(vector::at(mems, i) as type print_info** @, max_nlink_len, max_size_len);

	vector::delete_vector(dots);
	vector::delete_vector(mems);
	return 0;
}

func int ls_file(char* file, type std::syscall::stat statbuf) {
	using namespace std::syscall;
	using std::string::strlen;
	using std::lib::free;
	using std::lib::malloc;
	using std::lib::calloc;
	using std::string::strcpy;
	
	type print_info* pi = malloc(sizeof{type print_info}) as type print_info*;
	char* krwx = calloc((strlen(empty_krwx) + 1), sizeof{char}) as char*;
	strcpy(krwx, empty_krwx);
	update_type_statbuf(statbuf.st_mode, krwx);
	update_perms(krwx, statbuf);
	pi->perm_string = krwx;
	pi->nlink_string = ltoa(statbuf.st_nlink);
	pi->size_string = ltoa(statbuf.st_size);
	pi->name = malloc(sizeof{char} * (strlen(file) + 1)) as char*;
	pi->st_mode = statbuf.st_mode;
	strcpy(pi->name, file);

	display_print_info(pi, strlen(pi->nlink_string), strlen(pi->size_string));
}

func int main(int argc, char** argv) {
	using namespace std::syscall;
	using std::string::strlen;

	char* name;
	if(argc == 1) name = ".";
	else if(argc == 2) name = argv[1];
	else {
		direct_write(1, simple_ls_usage, strlen(simple_ls_usage));
		return 1;
	}

	type stat statbuf;
	int ret = direct_stat(name, statbuf$);
	if(ret == -1) {
		direct_write(1, missing_or_perm, strlen(missing_or_perm));
		return 1;
	}

	if(S_ISDIR(statbuf.st_mode))
		return ls_dir(name);
	else if(S_ISREG(statbuf.st_mode))
		return ls_file(name, statbuf);
	else {
		direct_write(1, file_or_directory, strlen(file_or_directory));
		return 1;
	}
	return 1;
}
