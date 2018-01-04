import <"std/lib">
import <"std/syscall">

func void fail() {
	std::syscall::direct_write(1, "N\n", 2);
	std::syscall::exit(1);
}

func void success() {
	std::syscall::direct_write(1, "Y\n", 2);
	std::syscall::exit(1);
}

func int main() {
	using std::lib::calloc;
	using std::lib::realloc;
	using std::lib::free;

	int* m1 = calloc(10, sizeof{int}) as int*;
	m1@ = 12;
	if(m1@ != 12) fail();
	for(int i = 1; i < 10; i++) if(m1[i] as bool) fail();
	m1 = realloc(m1 as byte*, 100) as int*;
	if(m1@ != 12) fail();
	for(int i = 1; i < 10; i++) if(m1[i] as bool) fail();
	m1 = realloc(m1 as byte*, 5 * sizeof{int}) as int*;
	if(m1@ != 12) fail();
	for(int i = 1; i < 5; i++) if(m1[i] as bool) fail();
	free(m1 as byte*);
	success();
}
