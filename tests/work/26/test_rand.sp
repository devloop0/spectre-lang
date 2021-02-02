import "../test.sp"
import <"std/lib">
import <"std/io">

using std::lib::rand;
using std::lib::srand;

constexpr unsigned int SEED = 234,
	NUM_CALLS = 100;

func void call_rand(unsigned int n) {
	for (unsigned int i = 0; i < n; i++) {
		int res = rand();
		if (rand() < 0) // make sure rand() returns an int greater than 0
			fail();
	}

}

func int main() {
	call_rand(NUM_CALLS);

	srand(SEED); // reseed the generator to something other than the default
	unsigned int f1_1 = rand(), f1_2 = rand(), f1_3 = rand(), f1_4 = rand();
	call_rand(NUM_CALLS);

	srand(SEED); // reseed it again to the same value
	unsigned int f2_1 = rand(), f2_2 = rand(), f2_3 = rand(), f2_4 = rand();
	call_rand(NUM_CALLS);

	// make sure that (at least) the first couple of generated values are deterministic
	// given the same initial seed.
	if (f1_1 != f2_1 || f1_2 != f2_2 || f1_3 != f2_3 || f1_4 != f2_4)
		fail();
	success();
}
