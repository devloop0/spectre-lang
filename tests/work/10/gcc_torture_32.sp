import <"std/syscall">

int** w = resv int*(2);

func int f() {
	for(int i = 0; ((i) < (2)); ((((i))++))) {
		for(int j = 0; ((j) < (2)); ((((j))++))) {
			if(((i) == (j)))
				((((((((((w)))[i])))[j]))) = (((1))));
		}
	}
}

func int main() {
	w@ = resv int(2), w[1] = resv int(2);
	f();
	if((((((((((w)))[0])))[0]))) != (1) || w[1][1] != 1 || w[1][0] != 0 || w[0][1] != 0) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(((1)), ((("Y\n"))), (2));
	return ((0));
}
