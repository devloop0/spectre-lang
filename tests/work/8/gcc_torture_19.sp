import <"std/syscall">

func int tar(long i) {
	if(i != 36863) {
		std::syscall::direct_write(1, "N\n", 2);
		std::syscall::exit(1);
		return 1;
	}
	return -1;
}

func void bug(int q, long bcount) {
	int j = 0, outgo = 0;
	while(j != -1) {
		outgo++;
		if(outgo > q - 1)
			outgo = q-1;
		j = tar(outgo*bcount);
	}
}

func int main() {
	(((bug)))(5, 36863);
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}
