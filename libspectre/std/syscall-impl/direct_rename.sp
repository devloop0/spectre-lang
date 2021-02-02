import <"std/syscall">

namespace std {
	namespace syscall {

		func int direct_rename(const char* oldpath, const char* newpath) {
			return do_syscall2(LINUX_ARM_RENAME, oldpath as int, newpath as int);
		}
	}
}
