import <"std/syscall">

func void adjust_xy(short* a, short* b);

struct adjust_template {
	short kx_x, kx_y, kx, kz;
}

static type adjust_template adjust;

func void init_adjust(type adjust_template* a) {
	a->kx_x = a->kx_y = 0;
	a->kx = a->kz = 1;
}

func int main() {
	init_adjust(adjust$);
	short x = 1, y = 1;
	adjust_xy(x$, y$);
	if(x != 1) std::syscall::direct_write(1, "N\n", 2);
	else std::syscall::direct_write(1, "Y\n", 2);
}

func void adjust_xy(short* x, short* y) {
	x@ = adjust.kx_x * x@ + adjust.kx_y * y@ + adjust.kx;
}
