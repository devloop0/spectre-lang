import <"std/syscall">
import <"std/lib">

func void fail() {
	using std::syscall::direct_write;
	using std::syscall::exit;
	
	direct_write(1, "N\n", 2);
	exit(-1);
}

func void success() {
	using std::syscall::direct_write;
	using std::syscall::exit;
	
	direct_write(1, "Y\n", 2);
	exit(0);
}

func int main() {
	using std::lib::malloc;
	using std::lib::free;

	int* m1 = malloc(100) as int*;
	for(int i = 0; i < 25; i++) m1[i] = i;
	int* m2 = malloc(100) as int*;
	for(int i = 0; i < 25; i++) m2[i] = i + 25;
	for(int i = 0; i < 25; i++) if(m1[i] != i) fail();
	free(m1 as byte*);
	for(int i = 0; i < 25; i++) if(m2[i] != i + 25) fail();
	free(m2 as byte*);

	char* m3 = malloc(50) as char*;
	for(int i = 0; i < 50; i++) m3[i] = i;
	char* m4 = malloc(50) as char*;
	for(int i = 0; i < 50; i++) m4[i] = i + 50;
	free(m4 as byte*);
	for(int i = 0; i < 50; i++) if(m3[i] != i) fail();
	free(m3 as byte*);
	
	success();
}
