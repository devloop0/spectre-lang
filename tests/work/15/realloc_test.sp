import <"std/syscall">
import <"std/lib">

func void fail() {
	using std::syscall::direct_write;
	using std::syscall::exit;

	direct_write(1, "N\n", 2);
	exit(1);
}

func void success() {
	using std::syscall::direct_write;
	using std::syscall::exit;

	direct_write(1, "Y\n", 2);
	exit(0);
}

func int main() {
	using std::lib::realloc;
	using std::syscall::direct_write;
	using std::lib::free;
	using std::lib::malloc;
	using std::lib::NULL;

	char* m1 = malloc(100) as char*;
	for(int i = 0; i < 100; i++) m1[i] = i;
	m1 = realloc(m1 as byte*, 200) as char*;
	for(int i = 0; i < 100; i++)
		if(m1[i] != i)
			fail();
	free(m1 as byte*);

	char* m2 = malloc(50) as char*, m3 = malloc(50) as char*;
	for(int i = 0; i < 50; i++) m2[i] = 100 + i, m3[i] = 200 + i;
	m2 = realloc(m2 as byte*, 100) as char*, m3 = realloc(m3 as byte*, 25) as char*;
	for(int i = 0; i < 50; i++)
		if(m2[i] != i + 100) fail();
	for(int i = 0; i < 25; i++)
		if(m3[i] != i + 200) fail();
	free(m3 as byte*), free(m2 as byte*);
	
	success();
}
