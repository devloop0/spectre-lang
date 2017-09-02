import <"std/syscall">

struct s { float f; double d; }

func int main() {
	type s ss, sss;
	ss.f = ss.d = 0;
	float one = 1, two = 2, mone = -1, mtwo = -2;
	double done = 1, dtwo = 2, dmone = -1, dmtwo = -2;
	type s* ssp = ss$, sssp = sss$;
	float* fa = [1_f], ga = [1_f];
	float f = 2;
	double *da = [1.], ea = [1.];
	double d = 2;
	ss.f = ss.d = sss.f = sss.d = 1;
	int ione = 1, itwo = 2, imone = -1, imtwo = -2;
	if((((((((true ? (((f))) : (((one))))))) != two))) || (false ? f : one) != -mone ||
		(true ? f : d) != 2 || (false ? f : d) != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if((true ? one : one) != -imone) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(1 != 1 || 1 != 1. || 1 != 1._f || 1 != 1_f ||
		1. != 1. || 1_f != 1_f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(one + 1. != f || one + 1 != two || d + done != f + 1. ||
		!(two <= -mtwo && two >= -mtwo) ||
		(two < f || two > -mtwo) || d + 1. != done + dtwo ||
		!(dtwo <= -dmtwo && dtwo >= -mtwo) ||
		(dtwo < d || -mtwo < dtwo) || f <= one || 1 >= f ||
		f <= done || d <= one || 1 >= d) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(-one != -1 || -one != -1._f || -mone != 1_f || -(-mone + 1) != -d ||
		-mone != 1 || -mtwo != f || -dtwo != -d || -dmone != 1 || 
		-dmone != -mone || -dmone != 1 || -mtwo != f ||
		-dtwo != dmtwo) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(f != 2 || f != 2._f || f != f || f != d ||
		d != 2 || d != 2._f || d != d || d != f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(ss.f != ss.d || ss.f != fa[0] || ss.f != d - 1 ||
		ss.f != ga[0] || ss.d != f - 1 || ss.d != fa[0] ||
		ss.d != ea[0] || ss.d != da[0] || ss.f != da[0] ||
		ss.d != ea[0] || ss.f != sss.f || ss.f != sssp->f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(ssp->f != ssp->d || ssp->f != fa[0] || ssp->f != d - 1 ||
		ssp->f != ga[0] || ssp->d != f - 1 || ssp->d != fa[0] ||
		ssp->d != ea[0] || ssp->d != da[0] || ssp->f != da[0] ||
		ssp->d != ea[0] || ss.d != sss.d || ss.d != sssp->d) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	ss.f = ss.d = 0;
	if(f != 2_f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(f != 2.) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(fa[0] != fa[0] || ga[0] != fa[0] || fa[0] != ga[0] ||
		ga[0] != ga[0] || f - 1 != fa[0] || d - 1 != ga[0] ||
		da[0] != ga[0] || f - 1. != fa@) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(fa@++ == f || fa@ != 2_f || fa@ != 2.) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(++ga@ != f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(da@++ == d || da@ != 2_f || da@ != 2.) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(++ea@ != d) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if((ssp->f = 1) != 1 || (ss.f -= 0) != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if((ssp->d = 1.) != 1 || (ss.d += 1) != 2) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if((1 as bool ? ss.f : 2) != 1) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if((1 as bool ? ss.f : 2.) != 1 ||
		((0 as bool ? ss.f : 2.) == 1)) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	ss.d = ss.f = sss.f = sss.d = 1;
	if(++ssp->f != f) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	if(sssp->f++ != one) {
		std::syscall::direct_write(1, "N\n", 2);
		return 1;
	}
	std::syscall::direct_write(1, "Y\n", 2);
	return 0;
}

