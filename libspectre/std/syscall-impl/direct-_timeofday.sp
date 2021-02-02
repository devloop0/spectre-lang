import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_gettimeofday(type timeval* tv, type timezone* tz) {
			return do_syscall2(LINUX_ARM_GETTIMEOFDAY, tv as int, tz as int);
		}

		func int direct_settimeofday(const type timeval* tv, const type timezone* tz) {
			return do_syscall2(LINUX_ARM_SETTIMEOFDAY, tv as int, tz as int);
		}
	}
}
