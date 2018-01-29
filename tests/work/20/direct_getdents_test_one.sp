import <"std/syscall">
import <"std/lib">
import <"std/string">

func char* which(char d_type) {
	using namespace std::syscall;
	switch(d_type) {
		case DT_REG: return "regular";
		case DT_FIFO: return "FIFO";
		case DT_CHR: return "char device";
		case DT_DIR: return "directory";
		case DT_BLK: return "block device";
		case DT_LNK: return "symbolic link";
		case DT_SOCK: return "socket";
		default: return "???";
	}
}

const static char* ten_dashes_nl = "----------\n";

func int main() {
	using namespace std::syscall;
	using namespace std::string;

	byte* buf = stk byte(1024);
	char* sep = ": ";
	char nl = '\n';

	int fd = direct_open(".", O_RDONLY | O_DIRECTORY);
	if(fd == -1) return 1;
	for( ; ; ) {
		int nread = direct_getdents(fd, buf, 1024);
		if(nread == -1) return 1;
		if(nread == 0) break;
		char* d_name = buf[LINUX_DIRENT_d_name_OFFSET]$ as char*;
		long d_off = buf[LINUX_DIRENT_d_off_OFFSET]$ as long* @;
		unsigned short d_reclen = buf[LINUX_DIRENT_d_reclen_OFFSET]$ as unsigned short* @;
		char d_type = buf[d_reclen - 1]$ as char* @;
		direct_write(1, d_name, strlen(d_name));
		direct_write(1, sep, 2);
		char* w = which(d_type);
		direct_write(1, w, strlen(w));
		direct_write(1, nl$, 1);
		direct_write(1, ten_dashes_nl, 11);
		direct_lseek(fd, d_off, SEEK_SET);
	}
	direct_close(fd);
}
