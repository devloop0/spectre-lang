import <"std/syscall">
import <"std/string">

constexpr unsigned int BUF_SIZE = 2;
const char* dne_1 = "File: '",
	dne_2 = "' could not be read.";

func int read_and_print(char* file_name) {
	using namespace std::syscall;
	using namespace std::string;

	char* local_buf = stk char(BUF_SIZE);
	char nl = '\n';

	int fd = direct_open(file_name, O_RDONLY);
	if(fd == -1) {
		direct_write(1, dne_1, strlen(dne_1));
		direct_write(1, file_name, strlen(file_name));
		direct_write(1, dne_2, strlen(dne_2));
		direct_write(1, nl$, 1);
		return 1;
	}

	memset(local_buf as byte*, 0, BUF_SIZE);
	int read_len;
	char last_char = 0;
	while((read_len = direct_read(fd, local_buf, BUF_SIZE - 1)) > 0) {
		direct_write(1, local_buf, read_len);
		last_char = local_buf[read_len - 1];
		memset(local_buf as byte*, 0, BUF_SIZE);
	}
	if(last_char != nl) direct_write(1, nl$, 1);
	return 0;
}

func int main(int argc, char** argv) {
	int ret_code = 0;

	for(unsigned int i = 1; i < argc; i++)
		ret_code = read_and_print(argv[i]);

	return ret_code;
}
