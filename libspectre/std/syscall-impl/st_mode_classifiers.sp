import <"std/syscall">

namespace std {
	namespace syscall {

		func bool S_ISREG(int m) { return (m & S_IFMT) == S_IFREG; }

		func bool S_ISDIR(int m) { return (m & S_IFMT) == S_IFDIR; }

		func bool S_ISCHR(int m) { return (m & S_IFMT) == S_IFCHR; }

		func bool S_ISBLK(int m) { return (m & S_IFMT) == S_IFBLK; }

		func bool S_ISFIFO(int m) { return (m & S_IFMT) == S_IFIFO; }

		func bool S_ISLNK(int m) { return (m & S_IFMT) == S_IFLNK; }

		func bool S_ISSOCK(int m) { return (m & S_IFMT) == S_IFSOCK; }
	}
}
