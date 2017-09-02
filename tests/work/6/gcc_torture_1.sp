import <"std/syscall">

func[static] int f(int a) {
	if(a == 0) return 0;
	do
		if((a & 128) as bool)
			return 1;
	while(f(0) as bool);
	return 0;
}

func int main() {
	if(f(~128) as bool) {
		std::syscall::direct_write(1, "N\n", 2);
		return 0;
	}
	std::syscall::direct_write(1, "Y\n", 2);
}
